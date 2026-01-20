#include <gtest/gtest.h>

#include <orderbook/Order.hpp>

TEST(OrderTest, TestOpenQuantity) {
    Order o{
        .id = 1,
        .side = Side::Buy,
        .price = 100.0,
        .total_quantity = 10,
        .filled_quantity = 0,
        .timestamp = 0
    };

    EXPECT_EQ(o.open_quantity(), 10);

    o.filled_quantity = 3;
    EXPECT_EQ(o.open_quantity(), 7);

    o.filled_quantity = 10;
    EXPECT_EQ(o.open_quantity(), 0);
}