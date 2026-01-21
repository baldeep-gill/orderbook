#include <gtest/gtest.h>

#include <orderbook/PriceLevel.hpp>
#include <orderbook/OrderPool.hpp>

TEST(PriceLevelTest, InsertIncreasesCorrectQuantity) {
    PriceLevel level;

    Order a {.id=1, .side=Side::Buy, .price=100.0, .total_quantity=10, .timestamp=1};
    Order b {.id=2, .side=Side::Buy, .price=100.0, .total_quantity=5, .timestamp=2};

    level.insert(&a);
    EXPECT_EQ(level.get_quantity(), 10);

    level.insert(&b);
    EXPECT_EQ(level.get_quantity(), 15);
}

TEST(PriceLevelTest, EraseDecreasesQuantityByRemainingAmount) {
    PriceLevel level;
    OrderPool pool;

    Order a {.id=1, .side=Side::Buy, .price=100.0, .total_quantity=10, .timestamp=1};
    auto itA = level.insert(&a);
    a.it = itA;

    EXPECT_EQ(level.get_quantity(), 10);

    level.match(4, pool);
    EXPECT_EQ(a.filled_quantity, 4);
    EXPECT_EQ(level.get_quantity(), 6);

    level.erase(a.it);
    EXPECT_EQ(level.get_quantity(), 0);
}

TEST(PriceLevelTest, TestMatchOrdering) {
    PriceLevel level;
    OrderPool pool;
    
    Order a {.id=1, .side=Side::Sell, .price=100.0, .total_quantity=5, .timestamp=1};
    Order b {.id=2, .side=Side::Sell, .price=100.0, .total_quantity=5, .timestamp=2};

    level.insert(&a);
    level.insert(&b);

    EXPECT_EQ(level.get_quantity(), 10);

    Quantity filled = level.match(7, pool);
    EXPECT_EQ(filled, 7);

    EXPECT_EQ(a.filled_quantity, 5);
    EXPECT_EQ(a.open_quantity(), 0);
    EXPECT_EQ(b.filled_quantity, 2);
    EXPECT_EQ(b.open_quantity(), 3);

    EXPECT_EQ(level.get_quantity(), 3);
    EXPECT_FALSE(level.empty());
}

TEST(PriceLevelTest, MatchConsumesLevel) {
    PriceLevel level;
    OrderPool pool;

    Order a {.id=1, .side=Side::Sell, .price=100.0, .total_quantity=4, .timestamp=1};
    Order b {.id=2, .side=Side::Sell, .price=100.0, .total_quantity=6, .timestamp=2};

    level.insert(&a);
    level.insert(&b);

    EXPECT_EQ(level.get_quantity(), 10);
    EXPECT_EQ(level.match(10, pool), 10);
    EXPECT_EQ(a.open_quantity(), 0);
    EXPECT_EQ(b.open_quantity(), 0);
    EXPECT_TRUE(level.empty());
    EXPECT_EQ(level.get_quantity(), 0);
}

TEST(PriceLevelTest, PartialMatchDoesNotRemovePartialFilledOrder) {
    PriceLevel level;
    OrderPool pool;

    Order a{.id=1, .side=Side::Buy, .price=100.0, .total_quantity=10, .timestamp=1};
    level.insert(&a);

    EXPECT_EQ(level.match(4, pool), 4);
    EXPECT_EQ(a.filled_quantity, 4);
    EXPECT_EQ(a.open_quantity(), 6);
    EXPECT_FALSE(level.empty());
    EXPECT_EQ(level.get_quantity(), 6);
}