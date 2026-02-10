#include <algorithm>
#include <iostream>
#include <vector>

#include "message_handler/TestMessageHandler.hpp"

void TestMessageHandler::handle_message(const ItchMessage& msg) {
    std::visit([&](auto&& m){ 
        stock_count_[bswap16(m.stock_locate)] += 1;

        MessageHandler::handle_message(m);
    }, msg);
}

const std::uint16_t TestMessageHandler::get_locate(const std::string& ticker) const {
    auto it = stock_locates_.begin();
    auto end = stock_locates_.end();
    while (it != end) {
        if (it->second == ticker) return it->first;
        ++it;
    }

    return 0;
}

void TestMessageHandler::print_counts() const {
    std::vector<std::pair<std::uint16_t, std::size_t>> values(stock_count_.begin(), stock_count_.end());
    std::sort(values.begin(), values.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    for (const auto& [locate, count]: values) {
        std::cout << MessageHandler::get_stock(locate) << ": " << count << "\n";
    }
}

void TestMessageHandler::print_locates() const {
    for (const auto& [locate, ticker]: stock_locates_) {
        std::cout << ticker << ": " << locate << "\n";
    }
}