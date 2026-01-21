#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Order.hpp"
#include "OrderPool.hpp"
#include "PriceLevel.hpp"
#include "ResultCode.hpp"

class OrderBook {
public:
    OrderBook() : orderpool_{}, bids_{}, asks_{}, orders_{}, order_lookup_{} {};

    OrderBook(OrderBook& other) = delete;
    void operator=(OrderBook& other) = delete;

    OrderBook(OrderBook&& other) = delete;
    void operator=(OrderBook&& other) = delete;

    ResultCode add_order(OrderId id, Side side, Price price, Quantity quantity);
    ResultCode cancel(OrderId id);
    ResultCode market_order(Side side, Quantity quantity);

    Price best_bid() const; // optional?
    Price best_ask() const;

private:
    OrderPool orderpool_;

    std::map<Price, PriceLevel, std::greater<Price>> bids_;
    std::map<Price, PriceLevel, std::less<Price>> asks_;
    
    std::vector<std::unique_ptr<Order>> orders_;

    std::unordered_map<OrderId, Order*> order_lookup_;

    Order* create_order(OrderId id, Side side, Price price, Quantity quantity);
    PriceLevel& get_or_create_level(Side side, Price price);
    void remove_order(Order* order);
    Quantity match(Side side, Price price, Quantity quantity);
    uint64_t get_timestamp();
};