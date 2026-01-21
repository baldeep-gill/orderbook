#include <gtest/gtest.h>

#include "orderbook/OrderBook.hpp"

class OrderBookTest : public testing::Test {
    protected:
    OrderBook book_;
    
    OrderBookTest() : book_{} {}
};

TEST(OrderBookTest, SimpleMatchTest) {
    OrderBook book{};

    book.add_order(0, Side::Buy, 100.0, 50);
    book.add_order(1, Side::Buy, 120.0, 10);
    ASSERT_EQ(book.best_bid(), 120);
    
    book.add_order(2, Side::Buy, 99.9, 50);    
    book.add_order(3, Side::Sell, 99.5, 60);

    ASSERT_EQ(book.best_bid(), 99.9);

    book.add_order(4, Side::Sell, 99.9, 50);

    ASSERT_EQ(book.best_ask(), 0);
    ASSERT_EQ(book.best_bid(), 0);
}

TEST(OrderBookTest, CheckEmptyBidAsk) {
    OrderBook book{};

    ASSERT_EQ(book.best_bid(), 0);
    ASSERT_EQ(book.best_ask(), 0);
}

TEST(OrderBookTest, AddSingleBid) {
    OrderBook book{};

    auto rc = book.add_order(1, Side::Buy, 100.0, 10);

    EXPECT_EQ(rc, ResultCode::Add_Success);
    EXPECT_EQ(book.best_bid(), 100);
    EXPECT_EQ(book.best_ask(), 0);
}

TEST(OrderBookTest, AddSingleAskSetsBestAsk) {
    OrderBook book{};

    auto rc = book.add_order(1, Side::Sell, 105, 5);
    EXPECT_EQ(rc, ResultCode::Add_Success);
    EXPECT_EQ(book.best_ask(), 105);
    EXPECT_EQ(book.best_bid(), 0);
}

TEST(OrderBookTest, TestBidPricePriority) {
    OrderBook book{};

    book.add_order(1, Side::Buy, 100, 10);
    book.add_order(2, Side::Buy, 101, 10);
    book.add_order(3, Side::Buy, 99, 10);
    
    EXPECT_EQ(book.best_bid(), 101);
}

TEST(OrderBookTest, TestAskPricePriority) {
    OrderBook book{};

    book.add_order(1, Side::Sell, 100, 10);
    book.add_order(2, Side::Sell, 101, 10);
    book.add_order(3, Side::Sell, 99, 10);
    
    EXPECT_EQ(book.best_ask(), 99);
}

TEST(OrderBookTest, CancelRemovesBestBid) {
    OrderBook book{};

    book.add_order(1, Side::Buy, 100, 10);
    book.add_order(2, Side::Buy, 101, 10);
    ASSERT_EQ(book.best_bid(), 101);

    auto rc = book.cancel(2);
    EXPECT_EQ(rc, ResultCode::Cancel_Success);
    ASSERT_EQ(book.best_bid(), 100);
}

TEST(OrderBookTest, CancelUnknownOrderIdFails) {
    OrderBook book;
    book.add_order(1, Side::Buy, 100, 10);

    auto rc = book.cancel(999);  // non-existent
    EXPECT_EQ(rc, ResultCode::Cancel_Fail);  // assert some error code
}

TEST(OrderBookTest, TestMarketBuyPriceTimePriority) {
    OrderBook book;

    book.add_order(1, Side::Sell, 100, 5);
    book.add_order(2, Side::Sell, 101, 5);

    auto rc = book.market_order(Side::Buy, 7);
    ASSERT_EQ(rc, ResultCode::Add_Success);

    EXPECT_EQ(book.best_ask(), 101);
    EXPECT_EQ(book.best_bid(), 0);
}

TEST(OrderBookTest, TestMarketOrderNoFill) {
    OrderBook book;

    auto rc = book.market_order(Side::Sell, 1);
    EXPECT_EQ(rc, ResultCode::Add_Fail);
}

TEST(OrderBookTest, DuplicateOrderIdRejection) {
    OrderBook book;

    EXPECT_EQ(book.add_order(1, Side::Buy, 100, 10), ResultCode::Add_Success);
    EXPECT_EQ(book.add_order(1, Side::Sell, 222, 2), ResultCode::Add_Fail);
}

TEST(OrderBookTest, ZeroQuantityRejected) {
    OrderBook book;

    EXPECT_EQ(book.add_order(1, Side::Buy, 100, 0), ResultCode::Add_Fail);
}

TEST(OrderBookTest, NegativeQuantityRejected) {
    OrderBook book;

    EXPECT_EQ(book.add_order(1, Side::Buy, 100, -5), ResultCode::Add_Fail);
}