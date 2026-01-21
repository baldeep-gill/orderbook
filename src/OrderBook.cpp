#include <chrono>

#include "orderbook/OrderBook.hpp"

ResultCode OrderBook::add_order(OrderId id, Side side, Price price, Quantity quantity) {
    if (order_lookup_.contains(id)) return ResultCode::Add_Fail;

    if (quantity <= 0) return ResultCode::Add_Fail;

    Quantity filled = match(side, price, quantity);
    Quantity remaining = quantity - filled;

    if (remaining) {
        Order* order = create_order(id ,side, price, remaining);
        auto& level = get_or_create_level(side, price);
        auto it = level.insert(order);
        order->it = it;

        if (remaining == quantity) return ResultCode::Add_Success;
        else return ResultCode::Add_PartialFill;
    }

    return ResultCode::Add_Fail;
}

ResultCode OrderBook::cancel(OrderId id) {
    auto it = order_lookup_.find(id);
    if (it != order_lookup_.end()) {
        Order* order = it->second;
        remove_order(order);
        order_lookup_.erase(id);

        return ResultCode::Cancel_Success;
    } else {
        return ResultCode::Cancel_Fail;
    }
}

ResultCode OrderBook::market_order(Side side, Quantity quantity) {
    Quantity filled = 0;

    if (side == Side::Buy) {

        auto it = asks_.begin();
        while (it != asks_.end() && quantity > 0) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty()) { it = asks_.erase(it); }
            else { ++it; }
        }

    } else { // Side::Sell

        auto it = bids_.begin();
        while (it != bids_.end() && quantity > 0) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty()) { it = bids_.erase(it); }
            else { ++it; }
        }
    }

    Quantity remaining = quantity - filled;

    return filled > 0 ? (remaining > 0 ? ResultCode::Add_PartialFill : ResultCode::Add_Success) 
                      : ResultCode::Add_Fail;
}

Price OrderBook::best_bid() const {
    return bids_.begin()->first;
}

Price OrderBook::best_ask() const {
    return asks_.begin()->first;
}

Order* OrderBook::create_order(OrderId id, Side side, Price price, Quantity quantity) {
    std::unique_ptr<Order> order = std::make_unique<Order>(id, side, price, quantity, 0, 0);
    Order* raw_ptr = order.get();
    orders_.push_back(std::move(order));
    order_lookup_[id] = raw_ptr;

    return raw_ptr;
}

PriceLevel& OrderBook::get_or_create_level(Side side, Price price) {
    if (side == Side::Buy) return bids_[price];
    else return asks_[price]; 
}

void OrderBook::remove_order(Order* order) {
    auto& level = get_or_create_level(order->side, order->price);
    level.erase(order->it);

    if (level.empty()) {
        (order->side == Side::Buy) ? bids_.erase(order->price) : asks_.erase(order->price);
    }
}

Quantity OrderBook::match(Side side, Price price, Quantity quantity) {
    Quantity filled = 0;

    std::vector<std::map<Price, PriceLevel>::iterator> to_erase{};

    if (side == Side::Buy) {
        if (price < best_ask()) return 0;

        auto end_it = asks_.upper_bound(price);
        for (auto it = asks_.begin(); it != end_it && quantity > 0; ++it) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty())  to_erase.push_back(it);
        }

        for (auto it: to_erase) asks_.erase(it);

        return filled;

    } else { 
        if (price > best_bid()) return 0;

        auto end_it = bids_.upper_bound(price);
        for (auto it = bids_.begin(); it != end_it && quantity > 0; ++it) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty()) to_erase.push_back(it); 
        }

        for (auto it: to_erase) bids_.erase(it);

        return filled;
    }
}

uint64_t OrderBook::get_timestamp() {
    return std::chrono::steady_clock::now().time_since_epoch().count();
}