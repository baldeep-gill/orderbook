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
        level.insert(order);

        if (remaining == quantity) return ResultCode::Add_Success;
        else return ResultCode::Add_PartialFill;
    }

    return ResultCode::Add_Fail;
}

ResultCode OrderBook::delete_order(OrderId id) {
    auto it = order_lookup_.find(id);
    if (it != order_lookup_.end()) {
        Order* order = it->second;
        remove_order(order);
        order_lookup_.erase(id);

        return ResultCode::Cancel_Success;  //TODO: Change result code name + test asserts
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

            Quantity level_filled = level.match(quantity, orderpool_);
            filled += level_filled;
            quantity -= level_filled;

            if (level.empty()) { it = asks_.erase(it); }
            else { ++it; }
        }

    } else { // Side::Sell

        auto it = bids_.begin();
        while (it != bids_.end() && quantity > 0) {
            auto& level = it->second;

            Quantity level_filled = level.match(quantity, orderpool_);
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
    Order* order = orderpool_.allocate();
    order->id = id;
    order->side = side;
    order->price = price;
    order->total_quantity = quantity;
    order->filled_quantity = 0;
    order->timestamp = 0;

    order_lookup_[id] = order;

    return order;
}

PriceLevel& OrderBook::get_or_create_level(Side side, Price price) {
    if (side == Side::Buy) return bids_[price];
    else return asks_[price]; 
}

void OrderBook::remove_order(Order* order) {
    auto& level = get_or_create_level(order->side, order->price);
    level.erase(order, orderpool_);

    if (level.empty()) {
        (order->side == Side::Buy) ? bids_.erase(order->price) : asks_.erase(order->price);
    }
}

template<OrderBookLevels Levels>
Quantity OrderBook::perform_match(Levels& levels, Price price, Quantity quantity) {
    Quantity filled = 0;

    std::vector<std::map<Price, PriceLevel>::iterator> to_erase{};

    auto end_it = levels.upper_bound(price);
    for (auto it = levels.begin(); it != end_it && quantity > 0; ++it) {
        auto& level = it->second;

        Quantity level_filled = level.match(quantity, orderpool_);
        filled += level_filled;
        quantity -= level_filled;
        
        if (level.empty()) to_erase.push_back(it);
    }

    for (auto it: to_erase) levels.erase(it);

    return filled;
}

Quantity OrderBook::match(Side side, Price price, Quantity quantity) {
    if (side == Side::Buy) {
        if (price < best_ask()) return 0;
        return perform_match(asks_, price, quantity);
    } else {
        if (price > best_bid()) return 0;
        return perform_match(bids_, price, quantity);
    }
}

uint64_t OrderBook::get_timestamp() {
    return std::chrono::steady_clock::now().time_since_epoch().count();
}