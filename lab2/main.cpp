#include <iostream>
#include <chrono>
#include <ctime>
#include <cstring>
#include <vector>
#include <cmath>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <x86intrin.h>
#include <numeric>
#include <algorithm>

constexpr int ITERATIONS = 1000000;
constexpr int RTC_MEASUREMENTS = 100;  // Уменьшено с 1000 до 100
constexpr int MAX_RTC_ATTEMPTS = 100;     // Увеличено с 10 до 100
constexpr int RTC_POLL_DELAY = 10000; // 10ms задержка между проверками

double tsc_per_ns = 0.0;



// Калибровка RDTSC
void calibrate_rdtsc() {
    constexpr int SAMPLES = 100;
    std::vector<double> ratios;
    ratios.reserve(SAMPLES);

    for(int i = 0; i < SAMPLES; ++i) {
        struct timespec start, end;
        uint64_t tsc1 = __rdtsc();
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        usleep(10000); // 10 ms
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t tsc2 = __rdtsc();
        
        double ns = (end.tv_sec - start.tv_sec)*1e9 + 
                   (end.tv_nsec - start.tv_nsec);
        if(ns > 0) {
            ratios.push_back((tsc2 - tsc1)/ns);
        }
    }
    
    tsc_per_ns = std::accumulate(ratios.begin(), ratios.end(), 0.0) / ratios.size();
}

void measure_rtc() {
    int fd = open("/dev/rtc", O_RDONLY); // Попробуйте /dev/rtc вместо /dev/rtc0
    if(fd < 0) {
        std::cerr << "Failed to open RTC: " << strerror(errno) << "\n";
        return;
    }
    std::vector<long> deltas;
    deltas.reserve(RTC_MEASUREMENTS);
    rtc_time prev, curr;
    if(ioctl(fd, RTC_RD_TIME, &prev) < 0) {
        std::cerr << "Initial RTC read failed: " << strerror(errno) << "\n";
        close(fd);
        return;
    }
    for(int i = 0; i < RTC_MEASUREMENTS; ++i) {
        int attempts = 0;
        auto t1 = std::chrono::high_resolution_clock::now();   
        do {
            if(ioctl(fd, RTC_RD_TIME, &curr) < 0) {
                std::cerr << "RTC read error: " << strerror(errno) << "\n";
                close(fd);
                return;
            }
            
            if(++attempts > MAX_RTC_ATTEMPTS) {
                std::cerr << "RTC timeout at measurement " << i << "\n";
                close(fd);
                return;
            }
            usleep(RTC_POLL_DELAY); // Добавлена задержка
            
        } while(memcmp(&prev, &curr, sizeof(rtc_time)) == 0);
        auto t2 = std::chrono::high_resolution_clock::now();
        deltas.push_back(
            std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
        );
        prev = curr; // Обновляем предыдущее значение
    }
    close(fd);
    // Статистика
    long sum = 0;
    for(auto d : deltas) sum += d;
    double mean = sum / (double)deltas.size();
    double variance = 0;
    for(auto d : deltas) variance += (d - mean)*(d - mean);
    double stddev = sqrt(variance/deltas.size());
    std::cout << "RTC Resolution:\n";
    std::cout << "  Mean access time: " << mean << " μs\n";
    std::cout << "  Std deviation:   " << stddev << " μs\n";
}

// Шаблон для измерения C++ clocks
template<typename Clock>
void measure_cpp_clock(const char* name) {
    std::vector<typename Clock::duration> deltas;
    deltas.reserve(ITERATIONS);

    auto start = Clock::now();
    for(int i = 0; i < ITERATIONS; ++i) {
        auto t1 = Clock::now();
        auto t2 = Clock::now();
        deltas.push_back(t2 - t1);
    }

    // Конвертация в наносекунды
    long long sum = 0;
    for(auto d : deltas) {
        sum += std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    }
    double mean = sum / (double)ITERATIONS;
    
    double variance = 0;
    for(auto d : deltas) {
        double ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
        variance += (ns - mean)*(ns - mean);
    }
    double stddev = sqrt(variance/ITERATIONS);
    
    std::cout << "\n" << name << ":\n";
    std::cout << "  Resolution:      " << mean << " ns\n";
    std::cout << "  Std deviation:   " << stddev << " ns\n";
}

// Измерение clock_gettime
void measure_posix_clock(clockid_t clk_id, const char* name) {
    struct timespec tp;
    std::vector<long> deltas;
    deltas.reserve(ITERATIONS);
    for(int i = 0; i < ITERATIONS; ++i) {
        timespec t1, t2;
        clock_gettime(clk_id, &t1);
        clock_gettime(clk_id, &t2);
        
        deltas.push_back((t2.tv_sec - t1.tv_sec)*1000000000L + 
                        (t2.tv_nsec - t1.tv_nsec));
    }
    long sum = 0;
    for(auto d : deltas) sum += d;
    double mean = sum / (double)ITERATIONS;
    double variance = 0;
    for(auto d : deltas) variance += (d - mean)*(d - mean);
    double stddev = sqrt(variance/ITERATIONS);
    std::cout << "\n" << name << ":\n";
    std::cout << "  Resolution:      " << mean << " ns\n";
    std::cout << "  Std deviation:   " << stddev << " ns\n";
}

// Измерение RDTSC
void measure_rdtsc() {
    // Калибровка
    auto t1 = std::chrono::high_resolution_clock::now();
    uint64_t start = __rdtsc();
    usleep(100000); // 100 ms
    uint64_t end = __rdtsc();
    auto t2 = std::chrono::high_resolution_clock::now();
    double ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    double cycles_per_ns = (end - start) / ns;
    // Измерение разрешения
    std::vector<uint64_t> deltas;
    for(int i = 0; i < ITERATIONS; ++i) {
        uint64_t t1 = __rdtsc();
        uint64_t t2 = __rdtsc();
        deltas.push_back(t2 - t1);
    }
    uint64_t sum = 0;
    for(auto d : deltas) sum += d;
    double mean = sum / (double)ITERATIONS;
    
    double variance = 0;
    for(auto d : deltas) variance += (d - mean)*(d - mean);
    double stddev = sqrt(variance/ITERATIONS);
    std::cout << "\nRDTSC:\n";
    std::cout << "  Cycles per ns:   " << cycles_per_ns << "\n";
    std::cout << "  Resolution:      " << mean/cycles_per_ns << " ns\n";
    std::cout << "  Std deviation:   " << stddev/cycles_per_ns << " ns\n";
}

// Измерение времени системного вызова
template<clockid_t CLK>
void measure_syscall_overhead() {
    constexpr int N = 1000000;
    std::vector<uint64_t> deltas(N);

    for(int i = 0; i < N; ++i) {
        uint64_t t1 = __rdtsc();
        timespec ts;
        clock_gettime(CLK, &ts);
        uint64_t t2 = __rdtsc();
        deltas[i] = t2 - t1;
    }
    
    // Расчет медианы
    std::nth_element(deltas.begin(), deltas.begin()+N/2, deltas.end());
    double median_cycles = deltas[N/2];
    double median_ns = median_cycles / tsc_per_ns;
    
    std::cout << "\nSyscall overhead for CLOCK_" 
              << (CLK == CLOCK_REALTIME ? "REALTIME" : "MONOTONIC")
              << ": " << median_ns << " ns\n";
}

int main() {
    std::cout << "Calibrating RDTSC...\n";
    calibrate_rdtsc();
    // Измерение C++ clocks
    measure_cpp_clock<std::chrono::system_clock>("system_clock");
    measure_cpp_clock<std::chrono::steady_clock>("steady_clock");
    measure_cpp_clock<std::chrono::high_resolution_clock>("high_resolution_clock");
    
    // Измерение POSIX clocks
    measure_posix_clock(CLOCK_REALTIME, "CLOCK_REALTIME");
    measure_posix_clock(CLOCK_MONOTONIC, "CLOCK_MONOTONIC");
    measure_posix_clock(CLOCK_MONOTONIC_RAW, "CLOCK_MONOTONIC_RAW");
    measure_posix_clock(CLOCK_PROCESS_CPUTIME_ID, "CLOCK_PROCESS_CPUTIME_ID");
    
    // Измерение RTC (требует прав root)
    measure_rtc();
    
    // Измерение RDTSC
    measure_rdtsc();
    measure_syscall_overhead<CLOCK_REALTIME>();
    measure_syscall_overhead<CLOCK_MONOTONIC>();
    measure_syscall_overhead<CLOCK_MONOTONIC_RAW>();
    measure_syscall_overhead<CLOCK_PROCESS_CPUTIME_ID>();
    return 0;
}