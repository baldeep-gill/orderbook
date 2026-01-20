#include <chrono>
#include <iostream>

#include "orderbook/OrderBook.hpp"

constexpr std::size_t N = 1'000'000;

void calculate_add_percentiles() {
    OrderBook book{};

    std::vector<long long> duration;
    duration.reserve(N);

    for (size_t i = 0; i < N; ++i) {
        auto start = std::chrono::steady_clock::now();

        book.add_order(i, Side::Buy, 100.0, 1);

        auto end = std::chrono::steady_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        duration.push_back(time);
    }

    std::sort(duration.begin(), duration.end());

    auto p50 = duration[N * 0.50];
    auto p95 = duration[N * 0.95];
    auto p99 = duration[N * 0.99];

    std::cout << "P50: " << p50 << "ns\n";
    std::cout << "P95: " << p95 << "ns\n";
    std::cout << "P99: " << p99 << "ns\n\n";
}

void calculate_add_throughput() {
    OrderBook book{};

    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < N; ++i) {
        book.add_order(i, Side::Buy, 100.0, 1);
    }

    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration<double>(end - start).count();
    long double tp = static_cast<long double>(N) / time;

    std::cout << "Throughput: " << tp << " orders/sec (" << time << "s total)\n\n"; 
}

void add_loop() {
    OrderBook book{};
    for (size_t i = 0; i < N; ++i) {
        book.add_order(i, Side::Buy, 100.0, 1);
    }
}

int main() {
    // calculate_add_percentiles();
    // calculate_add_throughput();

    add_loop();

    return 0;
}