#include "orderbook/OrderBook.hpp"

void OrderBook::add_order(OrderId id, Side side, Price price, Quantity quantity) {
    Order* order = create_order(id ,side, price, quantity);
    auto& level = get_or_create_level(side, price);
    auto it = level.insert(order);
    order->it = it;

    match(side, quantity);
}

void OrderBook::cancel(OrderId id) {
    auto it = order_lookup_.find(id);
    if (it != order_lookup_.end()) {
        Order* order = (*it).second;
        remove_order(order);
        order_lookup_.erase(id);
    }
}

Price OrderBook::best_bid() const {
    return (*bids_.begin()).first;
}

Price OrderBook::best_ask() const {
    return (*asks_.begin()).first;
}

Order* OrderBook::create_order(OrderId id, Side side, Price price, Quantity quantity) {
    std::unique_ptr<Order> ptr = std::make_unique<Order>();
    Order* order = ptr.get();

    order->id = id;
    order->side = side;
    order->price = price;
    order->quantity = quantity;
    order->timestamp = 0;

    orders_.push_back(ptr);
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
}

void OrderBook::match(Side side, Quantity quantity) {
}