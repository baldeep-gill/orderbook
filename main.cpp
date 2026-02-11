#include "message_handler/TestMessageHandler.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

#define swapEndian16(x) __builtin_bswap16(x)
#define swapEndian32(x) __builtin_bswap32(x)
#define swapEndian64(x) __builtin_bswap64(x)

#include "orderbook/OrderBook.hpp"
#include "message_handler/OrderBookMessageHandler.hpp"
#include "itch_parser/ItchParser.hpp"

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
    long double tp = N / time;

    std::cout << "Throughput: " << tp << " orders/sec (" << time << "s total)\n\n"; 
}

void add_loop() {
    OrderBook book{};
    for (size_t i = 0; i < N; ++i) {
        book.add_order(i, Side::Buy, 100.0, 1);
    }
}

void parse_binance() {
    OrderBook book{};

    uint64_t ids = 0;
    size_t count = 0;

    std::ifstream stream{"data/market_data.csv"};

    std::string line;
    std::getline(stream, line);

    auto start = std::chrono::steady_clock::now();
    while (std::getline(stream, line)) {
        std::istringstream ss{line};
        Order order;
        
        std::string ts_str, type_s, side_s, price_s, qty_s;

        std::getline(ss, ts_str, ',');
        std::getline(ss, type_s, ',');
        std::getline(ss, side_s, ',');
        std::getline(ss, price_s, ',');
        std::getline(ss, qty_s, ',');
        
        Side side = (side_s == "B") ? Side::Buy : Side::Sell;

        book.add_order(ids++, side, std::stoull(price_s), std::stoll(qty_s));
        ++count;
    }
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration<double>(end - start).count();

    std::cout << "Added " << count << " orders.\n" << "Took " << time << " seconds.\n";
    std::cout << "Bid: " << book.best_bid() << "\nAsk: " << book.best_ask() << "\n";
}

void benchmark_itch() {
    OrderBook book{};

    // TQQQ(9759): 5846895 messages
    // NVDA(7073): ~500k messages
    ItchParser<OrderBookMessageHandler> parser{std::make_unique<OrderBookMessageHandler>(book, 7073)};
    parser.parse_file();

    // const OrderBookMessageHandler& handler = parser.get_handler();
    // handler.print_counts();

    std::cout << "Bid: " << book.best_bid() << "\nAsk: " << book.best_ask() << "\n";
}

void test_message_handler() {
    ItchParser<TestMessageHandler> parser{std::make_unique<TestMessageHandler>()};
    parser.parse_file();

    const TestMessageHandler& handler = parser.get_handler();

    std::cout << handler.get_locate("NVDA") << std::endl;
}

int main() {
    calculate_add_percentiles();
    calculate_add_throughput();

    // parse_binance();
    
    // add_loop();

    // benchmark_itch();

    // test_message_handler();

    Order o{nullptr, nullptr, 0, Side::Sell, 150.0, 50, 0, 0};

    std::cout << sizeof(o) << "\n";

    return 0;
}