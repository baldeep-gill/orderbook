#pragma once

#include <cstdint>
#include <list>

using OrderId = std::uint64_t;
using Price = double;
using Quantity = std::int64_t;

enum class Side { Buy, Sell };

enum class OrderType { Limit, Market };

struct Order {
    OrderId id;
    Side side;
    Price price;
    Quantity total_quantity;
    Quantity filled_quantity{0};
    Quantity open_quantity() { return total_quantity - filled_quantity; }

    std::uint64_t timestamp;

    std::list<Order*>::iterator it;
};