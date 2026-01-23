#include <gtest/gtest.h>

#include <orderbook/PriceLevel.hpp>
#include <orderbook/OrderPool.hpp>

class PriceLevelTest : public ::testing::Test {
    protected:
        OrderPool pool_{1000};

        Order* alloc(Side side, Price price, Quantity qty) {
            Order* o = pool_.allocate();
            o->side = side;
            o->price = price;
            o->total_quantity = qty;

            return o;
        }
};

TEST_F(PriceLevelTest, SingleInsertErase) {
    PriceLevel level;

    Order* o1 = alloc(Side::Buy, 100, 10);
    level.insert(o1);

    EXPECT_EQ(level.quantity(), 10);
    EXPECT_EQ(level.length(), 1);
    EXPECT_FALSE(level.empty());
    EXPECT_EQ(level.head(), o1);
    
    level.erase(o1, pool_);
    EXPECT_EQ(level.quantity(), 0);
    EXPECT_EQ(level.length(), 0);
    EXPECT_TRUE(level.empty());
    EXPECT_EQ(level.head(), nullptr);
}

TEST_F(PriceLevelTest, InsertMultipleEraseMid) {
    PriceLevel level;

    Order* o1 = alloc(Side::Buy, 100, 10);
    Order* o2 = alloc(Side::Buy, 100, 20);
    Order* o3 = alloc(Side::Buy, 100, 30);

    level.insert(o1);
    level.insert(o2);
    level.insert(o3);

    EXPECT_EQ(level.quantity(), 60);
    EXPECT_EQ(level.length(), 3);
    EXPECT_EQ(level.head(), o1);
    
    level.erase(o2, pool_);
    EXPECT_EQ(level.quantity(), 40);
    EXPECT_EQ(level.length(), 2);
    EXPECT_EQ(level.head(), o1);
    EXPECT_EQ(o1->next, o3);
    EXPECT_EQ(o3->prev, o1);
}

TEST_F(PriceLevelTest, MatchPartialFullExhaust) {
    PriceLevel level;
    
    Order* o1 = alloc(Side::Buy, 100, 10);
    Order* o2 = alloc(Side::Buy, 100, 20);
    Order* o3 = alloc(Side::Buy, 100, 30);
    
    level.insert(o1);
    level.insert(o2);
    level.insert(o3);
    
    EXPECT_EQ(level.quantity(), 60);
    
    Quantity filled1 = level.match(5, pool_);
    EXPECT_EQ(filled1, 5);
    EXPECT_EQ(o1->filled_quantity, 5);
    EXPECT_EQ(o1->open_quantity(), 5);
    EXPECT_EQ(level.quantity(), 55);
    

    Quantity filled2 = level.match(20, pool_);
    EXPECT_EQ(filled2, 20);
    EXPECT_EQ(level.quantity(), 35);
    EXPECT_EQ(level.head(), o2);
    
    Quantity filled3 = level.match(100, pool_);
    EXPECT_EQ(filled3, 35);
    EXPECT_TRUE(level.empty());
}

TEST_F(PriceLevelTest, MatchZeroIncoming) {
    PriceLevel level;
    level.insert(alloc(Side::Buy, 100, 10));
    
    Quantity filled = level.match(0, pool_);
    EXPECT_EQ(filled, 0);
    EXPECT_FALSE(level.empty());
}

TEST_F(PriceLevelTest, EraseHead) {
    PriceLevel level;
    Order* o1 = alloc(Side::Buy, 100, 10);
    Order* o2 = alloc(Side::Buy, 100, 20);
    
    level.insert(o1);
    level.insert(o2);
    
    level.erase(o1, pool_);
    EXPECT_EQ(level.head(), o2);
    EXPECT_EQ(level.quantity(), 20);
}

TEST_F(PriceLevelTest, EraseTail) {
    PriceLevel level;
    Order* o1 = alloc(Side::Buy, 100, 10);
    
    level.insert(o1);
    level.erase(o1, pool_);
    EXPECT_TRUE(level.empty());
}

TEST_F(PriceLevelTest, EmptyMatch) {
    PriceLevel level;
    EXPECT_EQ(level.match(100, pool_), 0);
}

TEST_F(PriceLevelTest, LinksConsistentAfterOps) {
    PriceLevel level;
    Order* orders[3];
    for (int i = 0; i < 3; ++i) {
        orders[i] = alloc(Side::Buy, 100, 10);
        level.insert(orders[i]);
    }
    
    EXPECT_EQ(orders[0]->next, orders[1]);
    EXPECT_EQ(orders[1]->prev, orders[0]);

    EXPECT_EQ(orders[1]->next, orders[2]);
    EXPECT_EQ(orders[2]->prev, orders[1]);
    
    level.erase(orders[1], pool_);
    EXPECT_EQ(orders[0]->next, orders[2]);
    EXPECT_EQ(orders[2]->prev, orders[0]);
}

TEST_F(PriceLevelTest, InsertIncreasesCorrectQuantity) {
    PriceLevel level;
    
    Order* a = alloc(Side::Buy, 100.0, 10);
    Order* b = alloc(Side::Buy, 100.0, 5);
    
    level.insert(a);
    EXPECT_EQ(level.quantity(), 10);
    
    level.insert(b);
    EXPECT_EQ(level.quantity(), 15);
}

TEST_F(PriceLevelTest, EraseDecreasesQuantityByRemainingAmount) {
    PriceLevel level;
    
    Order* a = alloc(Side::Buy, 100.0, 10);
    level.insert(a);
    
    EXPECT_EQ(level.quantity(), 10);
    
    level.match(4, pool_);
    EXPECT_EQ(a->filled_quantity, 4);
    EXPECT_EQ(a->open_quantity(), 6);
    EXPECT_EQ(level.quantity(), 6);
    
    level.erase(a, pool_);
    EXPECT_EQ(level.quantity(), 0);
}

TEST_F(PriceLevelTest, TestMatchOrdering) {
    PriceLevel level;
    
    Order* a = alloc(Side::Sell, 100.0, 5);
    Order* b = alloc(Side::Sell, 100.0, 5);
    
    level.insert(a);
    level.insert(b);
    
    EXPECT_EQ(level.quantity(), 10);
    
    Quantity filled = level.match(7, pool_);
    EXPECT_EQ(filled, 7);
    
    EXPECT_EQ(a->filled_quantity, 5);
    EXPECT_EQ(a->open_quantity(), 0);
    EXPECT_EQ(b->filled_quantity, 2);
    EXPECT_EQ(b->open_quantity(), 3);
    
    EXPECT_EQ(level.quantity(), 3);
    EXPECT_FALSE(level.empty());
}

TEST_F(PriceLevelTest, MatchConsumesLevel) {
    PriceLevel level;
    
    Order* a = alloc(Side::Sell, 100.0, 4);
    Order* b = alloc(Side::Sell, 100.0, 6);
    
    level.insert(a);
    level.insert(b);
    
    EXPECT_EQ(level.quantity(), 10);
    EXPECT_EQ(level.match(10, pool_), 10);
    
    EXPECT_EQ(a->open_quantity(), 0);
    EXPECT_EQ(b->open_quantity(), 0);
    EXPECT_TRUE(level.empty());
    EXPECT_EQ(level.quantity(), 0);
}

TEST_F(PriceLevelTest, PartialMatchDoesNotRemovePartialFilledOrder) {
    PriceLevel level;
    
    Order* a = alloc(Side::Buy, 100.0, 10);
    level.insert(a);
    
    EXPECT_EQ(level.match(4, pool_), 4);
    EXPECT_EQ(a->filled_quantity, 4);
    EXPECT_EQ(a->open_quantity(), 6);
    EXPECT_FALSE(level.empty());
    EXPECT_EQ(level.quantity(), 6);
}