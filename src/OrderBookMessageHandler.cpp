#include <cstdint>

#include "message_handler/OrderBookMessageHandler.hpp"

void OrderBookMessageHandler::handle_message(const ItchMessage& msg) {
    std::visit([this](auto&& m){ 
        if (bswap16(m.stock_locate) != locate_) return;

        if constexpr (requires { process_message(m); }) process_message(m);
        else MessageHandler::process_message(m);

    }, msg);
}

void OrderBookMessageHandler::add_order(std::uint64_t id, char side, std::uint32_t price, std::uint32_t qty) {
    book_.add_order(
        bswap64(id),
        static_cast<Side>(side == 'S'),
        bswap32(price) / 10'000.0,
        bswap32(qty)
    );
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

