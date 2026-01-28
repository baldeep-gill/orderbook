#pragma once

#include "MessageHandler.hpp"
#include "orderbook/OrderBook.hpp"

class OrderBookMessageHandler : public MessageHandler {
    public:
        OrderBookMessageHandler(OrderBook& book) : book_{book}, stock_locates_{} {};
        ~OrderBookMessageHandler() = default;
    
        const std::string& get_stock(std::uint16_t locate); 

    private:
        OrderBook& book_;
        std::unordered_map<uint16_t, std::string> stock_locates_;
        
        std::string trim_spaces(const char* s, size_t size);
        void add_order(std::uint64_t id, char side, std::uint32_t price, std::uint32_t qty);
        
        void process_message(const Messages::R_StockDirectory&);
        void process_message(const Messages::A_AddOrder&);
        void process_message(const Messages::F_AddOrderMPID&);
        // void process_message(const Messages::E_OrderExecuted&);
        // void process_message(const Messages::C_OrderExecutedMessage&);
        void process_message(const Messages::X_OrderCancel&);
        void process_message(const Messages::D_OrderDelete&);
        void process_message(const Messages::U_OrderReplace&);
        
};