#include <iostream>

#include "orderbook/OrderBook.hpp"

int main() {
    OrderBook book{};

    book.add_order(0, Side::Buy, 100.0, 100);
    std::cout << book.best_bid() << "\n";

    auto rc = book.add_order(0 , Side::Buy, 120.0, 10);
    std::cout << static_cast<int>(rc) << "\n" << book.best_bid() << "\n";

    return 0;
}