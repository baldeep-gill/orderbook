#pragma once

#include "MessageHandler.hpp"
#include "orderbook/OrderBook.hpp"

class OrderBookMessageHandler : public MessageHandler {
    public:
        explicit OrderBookMessageHandler(OrderBook& book, std::uint16_t locate) : book_{book} {
            locate_ = locate;
        };
        ~OrderBookMessageHandler() = default;

        void handle_message(const ItchMessage&);
    
    private:
        OrderBook& book_;

        void add_order(std::uint64_t id, char side, std::uint32_t price, std::uint32_t qty);
        
        void process_message(const Messages::A_AddOrder&) override;
        void process_message(const Messages::F_AddOrderMPID&) override;
        // void process_message(const Messages::E_OrderExecuted&) override;
        // void process_message(const Messages::C_OrderExecutedMessage&) override;
        void process_message(const Messages::X_OrderCancel&) override;
        void process_message(const Messages::D_OrderDelete&) override;
        void process_message(const Messages::U_OrderReplace&) override;
        
};