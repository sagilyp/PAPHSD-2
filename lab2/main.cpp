#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <x86intrin.h>
#include <ctime>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <stdexcept>
//#include <typeinfo>
//#include <type_traits>

using namespace std;
using namespace std::chrono;

constexpr size_t ITERATIONS = 1'000'000;
constexpr size_t RTC_SAMPLES = 100;
constexpr size_t CALIBRATION_SAMPLES = 100;
constexpr int MAX_RTC_ATTEMPTS = 1000;

struct TimerCharacteristics {
    double resolution;       // Экспериментальное разрешение
    double resolution_error; // Погрешность разрешения
    double sys_accuracy;     // Точность по системному вызову
    double a1;               // Время инициализации
    double a2;               // Время возврата
};

class TimeProfiler {
    double tsc_per_ns;
    vector<clockid_t> posix_clocks;
    
    void calibrate_tsc() {
        vector<double> ratios;
        for (size_t i = 0; i < CALIBRATION_SAMPLES; ++i) {
            timespec start, end;
            uint64_t tsc1 = __rdtsc();
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            usleep(1000);
            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
            uint64_t tsc2 = __rdtsc();
            
            double ns = (end.tv_sec - start.tv_sec)*1e9 + (end.tv_nsec - start.tv_nsec);
            ratios.push_back((tsc2 - tsc1) / ns);
        }
        tsc_per_ns = accumulate(ratios.begin(), ratios.end(), 0.0) / ratios.size();
    }

    double rdtsc_to_ns(uint64_t cycles) const {
        return cycles / tsc_per_ns;
    }

    template<typename Clock>
    TimerCharacteristics measure_cpp_clock() {
        vector<uint64_t> deltas;
        vector<uint64_t> a1_samples;
        vector<uint64_t> a2_samples;

        for (size_t i = 0; i < ITERATIONS; ++i) {
            uint64_t t1 = __rdtsc();
            auto now = Clock::now();
            uint64_t t2 = __rdtsc();
            auto now2 = Clock::now();
            uint64_t t3 = __rdtsc();
            
            deltas.push_back(t2 - t1);
            a1_samples.push_back(t2 - t1);
            a2_samples.push_back(t3 - t2);

            asm volatile("" : : "r"(now), "r"(now2) : "memory");
        }

        double mean = accumulate(deltas.begin(), deltas.end(), 0.0) / ITERATIONS;
        double sq_sum = inner_product(deltas.begin(), deltas.end(), deltas.begin(), 0.0);
        double stdev = sqrt(sq_sum/ITERATIONS - mean*mean);

        // Получение системной точности через POSIX
        constexpr clockid_t clk = 
            is_same_v<Clock, system_clock> ? CLOCK_REALTIME :
            is_same_v<Clock, steady_clock> ? CLOCK_MONOTONIC :
            CLOCK_MONOTONIC_RAW;
            
        timespec res;
        clock_getres(clk, &res);
        double sys_acc = res.tv_sec*1e9 + res.tv_nsec;

        // Расчет A1/A2
        double a1 = accumulate(a1_samples.begin(), a1_samples.end(), 0.0) / ITERATIONS;
        double a2 = accumulate(a2_samples.begin(), a2_samples.end(), 0.0) / ITERATIONS;

        return {
            rdtsc_to_ns(mean),
            rdtsc_to_ns(stdev),
            sys_acc,
            rdtsc_to_ns(a1),
            rdtsc_to_ns(a2)
        };
    }

public:
    TimeProfiler() {
        calibrate_tsc();
        posix_clocks = {
            CLOCK_REALTIME, 
            CLOCK_MONOTONIC,
            CLOCK_MONOTONIC_RAW,
            CLOCK_PROCESS_CPUTIME_ID,
            CLOCK_THREAD_CPUTIME_ID
        };
    }

    TimerCharacteristics measure_rtc() {
        int fd = open("/dev/rtc0", O_RDONLY);
        if (fd < 0) throw runtime_error("RTC open failed");
        vector<uint64_t> a1_samples;
        vector<uint64_t> a2_samples;
        vector<uint64_t> deltas;
        rtc_time prev, curr;
        
        if (ioctl(fd, RTC_RD_TIME, &prev) < 0) {
            close(fd);
            throw runtime_error("RTC read failed");
        }

        for (size_t i = 0; i < RTC_SAMPLES; ++i) {
            int attempts = 0;
                    // Измерение A1
            uint64_t t1 = __rdtsc();
            if (ioctl(fd, RTC_RD_TIME, &curr) < 0) {
                close(fd);
                throw runtime_error("RTC read failed");
            }
            uint64_t t2 = __rdtsc();
            a1_samples.push_back(t2 - t1);

            // Измерение A2
            uint64_t t3 = __rdtsc();
            if (ioctl(fd, RTC_RD_TIME, &prev) < 0) { // Фиктивный вызов для A2
                close(fd);
                throw runtime_error("RTC read failed");
            }
            uint64_t t4 = __rdtsc();
            a2_samples.push_back(t4 - t3);
            uint64_t t5 = __rdtsc();
            do {
                if (ioctl(fd, RTC_RD_TIME, &curr) < 0) {
                    close(fd);
                    throw runtime_error("RTC read failed");
                }
                if (++attempts > MAX_RTC_ATTEMPTS) {
                    close(fd);
                    throw runtime_error("RTC timeout");
                }
                usleep(1000);
            } while (memcmp(&prev, &curr, sizeof(rtc_time)) == 0);

            uint64_t t6 = __rdtsc();
            deltas.push_back(t6 - t5);
            prev = curr;
        }
        close(fd);

        double mean = accumulate(deltas.begin(), deltas.end(), 0.0) / deltas.size();
        double sq_sum = inner_product(deltas.begin(), deltas.end(), deltas.begin(), 0.0);
        double stdev = sqrt(sq_sum/deltas.size() - mean*mean);
        double a1 = accumulate(a1_samples.begin(), a1_samples.end(), 0.0) / a1_samples.size() / tsc_per_ns;
        double a2 = accumulate(a2_samples.begin(), a2_samples.end(), 0.0) / a2_samples.size() / tsc_per_ns;
        return {
            rdtsc_to_ns(mean),
            rdtsc_to_ns(stdev),
            1e6, // 1 ms для RTC
            a1, a2 // A1/A2 не измеряем для RTC
        };
    }

    TimerCharacteristics measure_posix_clock(clockid_t clk) {
        // Измерение системной точности
        timespec res;
        clock_getres(clk, &res);
        double sys_acc = res.tv_sec*1e9 + res.tv_nsec;

        // Измерение разрешения
        vector<uint64_t> deltas;
        for (size_t i = 0; i < ITERATIONS; ++i) {
            timespec t1, t2;
            uint64_t s1 = __rdtsc();
            clock_gettime(clk, &t1);
            clock_gettime(clk, &t2);
            uint64_t s2 = __rdtsc();
            
            deltas.push_back(rdtsc_to_ns(s2 - s1));
        }

        // Статистика
        double mean = accumulate(deltas.begin(), deltas.end(), 0.0) / ITERATIONS;
        double sq_sum = inner_product(deltas.begin(), deltas.end(), deltas.begin(), 0.0);
        double stdev = sqrt(sq_sum/ITERATIONS - mean*mean);

        // Измерение A1/A2
        vector<pair<uint64_t, uint64_t>> ab_samples;
        for (size_t i = 0; i < ITERATIONS; ++i) {
            uint64_t t1 = __rdtsc();
            timespec dummy;
            clock_gettime(clk, &dummy);
            uint64_t t2 = __rdtsc();
            clock_gettime(clk, &dummy);
            uint64_t t3 = __rdtsc();
            ab_samples.emplace_back(t2 - t1, t3 - t2);
        }

        double a = accumulate(ab_samples.begin(), ab_samples.end(), 0.0,
            [this](double sum, auto& p) { return sum + rdtsc_to_ns(p.first); }) / ITERATIONS;
        
        double b = accumulate(ab_samples.begin(), ab_samples.end(), 0.0,
            [this](double sum, auto& p) { return sum + rdtsc_to_ns(p.second); }) / ITERATIONS;

        return {mean, stdev, sys_acc, a, b};
    }


    TimerCharacteristics measure_rdtsc() {
        vector<uint64_t> deltas;
        vector<uint64_t> a1_samples;
        vector<uint64_t> a2_samples;
        for (size_t i = 0; i < ITERATIONS; ++i) {
            uint64_t t1 = __rdtsc();
            uint64_t t2 = __rdtsc();
            uint64_t t3 = __rdtsc();
            deltas.push_back(t2 - t1);
            a1_samples.push_back(t2 - t1);
            a2_samples.push_back(t3 - t2);
        }

        double mean = accumulate(deltas.begin(), deltas.end(), 0.0) / ITERATIONS;
        double sq_sum = inner_product(deltas.begin(), deltas.end(), deltas.begin(), 0.0);
        double stdev = sqrt(sq_sum/ITERATIONS - mean*mean);
        double a1 = accumulate(a1_samples.begin(), a1_samples.end(), 0.0) / ITERATIONS;
        double a2 = accumulate(a2_samples.begin(), a2_samples.end(), 0.0) / ITERATIONS;

        return {
            mean / tsc_per_ns,
            stdev / tsc_per_ns,
            1.0 / tsc_per_ns, // Теоретическая точность
            a1/tsc_per_ns,
            a2/tsc_per_ns              // Нет системных вызовов
        };
    }

    void print_table() {
        cout << "| Timer Name               | Resolution (ns) | Error (ns) | Sys Accuracy (ns) | A1 (ns) | A2 (ns) |\n";
        cout << "|--------------------------|-----------------|------------|--------------------|---------|---------|\n";
        
        try {
            auto rtc = measure_rtc();
            printf("| %-24s | %9.1f      | %7.1f    | %12.1f       | %6.1f   | %6.1f   |\n",
                   "RTC", rtc.resolution, rtc.resolution_error, rtc.sys_accuracy, rtc.a1, rtc.a2);
        } catch (const exception& e) {
            cerr << "RTC measurement failed: " << e.what() << endl;
        }

        try {
            auto sys = measure_cpp_clock<std::chrono::_V2::system_clock>();
            printf("| %-24s | %9.1f      | %7.1f    | %12.1f       | %6.1f  | %6.1f   |\n",
                       "system_clock", sys.resolution, sys.resolution_error, sys.sys_accuracy, sys.a1, sys.a2);
        }catch (const exception& e) {
            cerr << "system_clock measurement failed: " << e.what() << endl;
        }
    
            try {
                auto steady = measure_cpp_clock<steady_clock>();
                printf("| %-24s | %9.1f      | %7.1f    | %12.1f       | %6.1f    | %6.1f    |\n",
                       "steady_clock", steady.resolution, steady.resolution_error, steady.sys_accuracy, steady.a1, steady.a2);
            }catch (const exception& e) {
                cerr << "steady_clock measurement failed: " << e.what() << endl;
            }
    
            try {
                auto hrc = measure_cpp_clock<high_resolution_clock>();
                printf("| %-24s | %9.1f      | %7.1f    | %12.1f       | %6.1f    | %6.1f    |\n",
                       "high_resolution_clock", hrc.resolution, hrc.resolution_error, hrc.sys_accuracy, hrc.a1, hrc.a2);
            }catch (const exception& e) {
                cerr << "high_resolution_clock measurement failed: " << e.what() << endl;
            }
        

        for (auto clk : posix_clocks) {
            try {
                auto res = measure_posix_clock(clk);
                const char* name = "";
                switch(clk) {
                    case CLOCK_REALTIME: name = "CLOCK_REALTIME"; break;
                    case CLOCK_MONOTONIC: name = "CLOCK_MONOTONIC"; break;
                    case CLOCK_MONOTONIC_RAW: name = "CLOCK_MONOTONIC_RAW"; break;
                    case CLOCK_PROCESS_CPUTIME_ID: name = "CLOCK_PROCESS_CPUTIME"; break;
                    case CLOCK_THREAD_CPUTIME_ID: name = "CLOCK_THREAD_CPUTIME"; break;
                }
                printf("| %-24s | %9.1f      | %7.1f    | %12.1f       | %6.1f | %6.1f |\n",
                       name, res.resolution, res.resolution_error, res.sys_accuracy, res.a1, res.a2);
            } catch (...) {
                cerr << "Measurement failed for clock: " << clk << endl;
            }
        }

        try {
            auto rdtsc_res = measure_rdtsc();
            printf("| %-24s | %9.1f      | %7.1f    | %12.1f       | %6.1f    | %6.1f    |\n",
                   "RDTSC", rdtsc_res.resolution, rdtsc_res.resolution_error, rdtsc_res.sys_accuracy, rdtsc_res.a1, rdtsc_res.a2);
        } catch (...) {
            cerr << "RDTSC measurement failed" << endl;
        }
    }
};

int main() {
    try {
        TimeProfiler profiler;
        profiler.print_table();
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }
    return 0;
}