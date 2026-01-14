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
    OrderType type;
    Price price;
    Quantity quantity;

    std::uint64_t timestamp;

    std::list<Order*>::iterator it;
};