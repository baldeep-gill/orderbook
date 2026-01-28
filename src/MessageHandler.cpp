#include "message_handler/OrderBookMessageHandler.hpp"

std::string OrderBookMessageHandler::trim_spaces(const char* s, size_t size) {
    std::string result(s, size);
    for (size_t i = 0; i < size; ++i) {
        if (s[i] == ' ') result.resize(i);
    }

    return result;
}

void OrderBookMessageHandler::add_order(std::uint64_t id, char side, std::uint32_t price, std::uint32_t qty) {
    book_.add_order(
        __builtin_bswap64(id),
        static_cast<Side>(side == 'S'),
        __builtin_bswap32(price) / 10'000.0,
        __builtin_bswap32(qty)
    );
}

void OrderBookMessageHandler::process_message(const Messages::R_StockDirectory& msg) {
    std::string ticker = trim_spaces(msg.stock, 8);
    stock_locates_[__builtin_bswap16(msg.stock_locate)] = ticker;
}

void OrderBookMessageHandler::process_message(const Messages::A_AddOrder& msg) {
    add_order(msg.order_reference, msg.side, msg.price, msg.shares);
}

void OrderBookMessageHandler::process_message(const Messages::F_AddOrderMPID& msg) {
    add_order(msg.order_reference, msg.side, msg.price, msg.shares);
}