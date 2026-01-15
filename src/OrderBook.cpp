#include <chrono>

#include "orderbook/OrderBook.hpp"

ResultCode OrderBook::add_order(OrderId id, Side side, Price price, Quantity quantity) {
    Quantity filled = match(side, price, quantity);
    Quantity remaining = quantity - filled;

    if (remaining) {
        //partial fill
        Order* order = create_order(id ,side, price, remaining);
        auto& level = get_or_create_level(side, price);
        auto it = level.insert(order);
        order->it = it;

        return ResultCode::Add_PartialFill;
    } else {
        // fully filled
        return ResultCode::Add_Success;
    }
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
    std::unique_ptr<Order> ptr = std::make_unique<Order>();
    Order* order = ptr.get();

    order->id = id;
    order->side = side;
    order->price = price;
    order->total_quantity = quantity;
    order->filled_quantity = 0;
    order->timestamp = get_timestamp();

    orders_.push_back(std::move(ptr));
    order_lookup_[id] = order;

    return order;
}

PriceLevel& OrderBook::get_or_create_level(Side side, Price price) {
    if (side == Side::Buy) return bids_[price];
    else return asks_[price]; 
}

void OrderBook::remove_order(Order* order) {
    auto& level = get_or_create_level(order->side, order->price);
    level.erase(order->it);

    if (level.empty()) {
        if (order->side == Side::Buy ? bids_.erase(order->price) : asks_.erase(order->price));
    }
}

Quantity OrderBook::match(Side side, Price price, Quantity quantity) {
    Quantity filled = 0;

    if (side == Side::Buy) {

        if (price < best_ask()) { // no match, early return
            return 0;
        }

        auto it = asks_.begin();
        while (it != asks_.end() && quantity > 0 && it->first <= price) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty()) { it = asks_.erase(it); }
            else { ++it; }
        }

        return filled;
    } else { // Side::Sell
        if (price > best_bid()) {
            return 0;
        }

        auto it = bids_.begin();
        while (it != bids_.end() && quantity > 0 && it->first >= price) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty()) { it = bids_.erase(it); }
            else { ++it; }
        }

        return filled;
    }
}

uint64_t OrderBook::get_timestamp() {
    return std::chrono::steady_clock::now().time_since_epoch().count();
}