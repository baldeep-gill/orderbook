#include <algorithm>
#include <cstdint>
#include <iostream>

#include "message_handler/OrderBookMessageHandler.hpp"

const std::string OrderBookMessageHandler::get_stock(std::uint16_t locate) const {
    if (stock_locates_.contains(locate)) return stock_locates_.at(locate);
    else return "error";
}

void OrderBookMessageHandler::print_counts() const {
    std::vector<std::pair<std::uint16_t, std::size_t>> values(stock_count_.begin(), stock_count_.end());
    std::sort(values.begin(), values.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    for (const auto& [locate, count]: values) {
        std::cout << get_stock(locate) << ": " << count << "\n"; 
    }
}

std::string OrderBookMessageHandler::trim_spaces(const char* s, size_t size) {
    std::string result(s, size);
    for (size_t i = 0; i < size; ++i) {
        if (s[i] == ' ') result = result.substr(0, i);
    }

    return result;
}

void OrderBookMessageHandler::add_order(std::uint64_t id, char side, std::uint32_t price, std::uint32_t qty) {
    book_.add_order(
        bswap64(id),
        static_cast<Side>(side == 'S'),
        bswap32(price) / 10'000.0,
        bswap32(qty)
    );
}

void OrderBookMessageHandler::process_message(const Messages::R_StockDirectory& msg) {
    std::string ticker = trim_spaces(msg.stock, 8);
    stock_locates_[bswap16(msg.stock_locate)] = ticker;
}

void OrderBookMessageHandler::process_message(const Messages::A_AddOrder& msg) {
    add_order(msg.order_reference, msg.side, msg.price, msg.shares);
}

void OrderBookMessageHandler::process_message(const Messages::F_AddOrderMPID& msg) {
    add_order(msg.order_reference, msg.side, msg.price, msg.shares);
}

void OrderBookMessageHandler::process_message(const Messages::X_OrderCancel& msg) {
    book_.cancel_order(bswap64(msg.reference_number), bswap32(msg.shares_cancelled));
}

void OrderBookMessageHandler::process_message(const Messages::D_OrderDelete& msg) {
    book_.delete_order(bswap64(msg.reference_number));
}

void OrderBookMessageHandler::process_message(const Messages::U_OrderReplace& msg) {
    book_.replace_order(
        bswap64(msg.original_reference),
        bswap64(msg.new_reference),
        bswap32(msg.price) / 10'000.0,
        bswap32(msg.shares)
    );
}

