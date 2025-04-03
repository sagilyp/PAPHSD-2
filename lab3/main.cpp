#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <numeric>

constexpr int SEED = 42;

// Простая реализация линейного конгруэнтного генератора случайных чисел
class RandomGenerator {
private:
    unsigned long a, c, m, state;
public:
    RandomGenerator(unsigned long seed, unsigned long a = 1664525, unsigned long c = 1013904223, unsigned long m = 4294967296)
        : a(a), c(c), m(m), state(seed) {}
    unsigned long next() {
        state = (a * state + c) % m;
        return state;
    }
};

void measure_time(int k, std::ifstream& file_in, std::streamsize file_size) {
    const int block_size = 8; 
    const int num_reads = 1000000; 
    if (file_size < k) {
        std::cerr << "Размер k превышает размер файла! Пропускаем k = " << k << std::endl;
        return;
    }
    RandomGenerator rng(SEED); 
    unsigned long a = rng.next() % (file_size - k);
    std::cout << "Используем фиксированное смещение a = " << a << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    unsigned long sum = 0;
    for (int i = 0; i < num_reads; ++i) {
        unsigned long offset = a + (rng.next() % k);
        if (offset + block_size > file_size) {
            continue;
        }
        file_in.seekg(offset, std::ios::beg);
        char buffer[block_size];
        file_in.read(buffer, block_size);
        for (int j = 0; j < block_size; ++j) {
            sum += static_cast<unsigned char>(buffer[j]); 
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time for k = " << k << ": " << duration.count() << " seconds, checksum = " << sum << std::endl;
}

int main() {
    const std::string filename = "/home/sagilyp/2025-03-22 10-33-05.mkv";
    std::ifstream file_in(filename, std::ios::binary | std::ios::ate);
    if (!file_in) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return 1;
    }
    std::streamsize file_size = file_in.tellg();
    file_in.seekg(0, std::ios::beg); // Сбрасываем указатель на начало файла
    std::cout << "Размер файла: " << file_size << " байт" << std::endl;
    std::vector<int> ks = {24000, 30000, 48000, 80000, 200000, 500000, 1000000, 
                           5000000, 8000000, 20000000, 40000000, 80000000}; 
    for (int k : ks) {
        measure_time(k, file_in, file_size);
    }
    file_in.close();
    return 0;
}
