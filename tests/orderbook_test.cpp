#include <gtest/gtest.h>

#include "orderbook/OrderBook.hpp"

class OrderBookTest : public testing::Test {
    protected:
    OrderBook book_;
    
    OrderBookTest() : book_{} {}
};

TEST_F(OrderBookTest, SimpleMatchTest) {
    book_.add_order(0, Side::Buy, 100.0, 100);

    book_.add_order(1 , Side::Buy, 120.0, 10);

    book_.cancel(1);    

    book_.add_order(2, Side::Sell, 99.5, 50);

    ASSERT_EQ(book_.best_bid(), 100);
}