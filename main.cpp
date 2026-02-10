#include <chrono>
#include <iostream>
#include <fstream>
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

// template<class... Ts>
// struct overloaded : Ts... { 
//     using Ts::operator()...; 
// };

// void process_add_order(OrderBook& book, std::uint64_t order_ref, char side, std::uint32_t price, std::uint32_t qty) {
//     book.add_order(
//         swapEndian64(order_ref),
//         static_cast<Side>(side == 'S'),
//         swapEndian32(price) / 10'000.0,
//         swapEndian32(qty)
//     );
// }

// std::string_view trim_spaces(std::string_view s, size_t size) {
//     for (size_t i = 0; i < size; ++i) {
//         if (s.at(i) == ' ') return s.substr(0, i);
//     }

//     return s;
// }

// void benchmark_itch() {
//     OrderBook book{};
//     size_t count{0};
//     std::uint16_t aapl_locate{};    // Big endian!

//     auto handler_dispatch = overloaded{
//         [&](const Messages::R_StockDirectory& m) {
//             auto s = trim_spaces(m.stock, 8);
            
//             if (s == "AAPL") {
//                 aapl_locate = m.stock_locate;
//             }
//         },
//         [&](const Messages::A_AddOrder& m) { 
//             if (m.stock_locate == aapl_locate) {
//                 process_add_order(book, m.order_reference, m.side, m.price, m.shares);
//                 ++count;
//             }
//          },
//         [&](const Messages::F_AddOrderMPID& m) { 
//             if (m.stock_locate == aapl_locate) {
//                 process_add_order(book, m.order_reference, m.side, m.price, m.shares);
//                 ++count;
//             }
//          },
//         [&](const auto& m) {}
//     };

//     auto cb = [&](const ItchMessage& msg) {
//         std::visit(handler_dispatch, msg);
//     };

//     ItchParser parser{cb};
//     parser.parse_file();
//     std::cout << "Count: " << count << "\n";
//     std::cout << "Bid: " << book.best_bid() << "\nAsk: " << book.best_ask() << "\n";
// }

void benchmark_itch() {
    OrderBook book{};

    // TQQQ: 5846895
    // TQQQ is the ticker with the most messages related to it
    ItchParser<OrderBookMessageHandler> parser{std::make_unique<OrderBookMessageHandler>(book, 9759)};
    parser.parse_file();

    const OrderBookMessageHandler& handler = parser.get_handler();
    // handler.print_counts();

    std::cout << "Bid: " << book.best_bid() << "\nAsk: " << book.best_ask() << "\n";
}

int main() {
    // calculate_add_percentiles();
    // calculate_add_throughput();

    // parse_binance();
    
    // add_loop();

    benchmark_itch();

    return 0;
}