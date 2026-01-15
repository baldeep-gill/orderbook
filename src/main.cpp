#include <iostream>

#include "orderbook/OrderBook.hpp"

int main() {
    OrderBook book{};

    book.add_order(0, Side::Buy, 100.0, 100);
    std::cout << book.best_bid() << "\n";

    book.add_order(1 , Side::Buy, 120.0, 10);
    std::cout << book.best_bid() << "\n";

    book.cancel(1);    
    std::cout << book.best_bid() << "\n";

    book.add_order(2, Side::Sell, 99.5, 50);
    std::cout << book.best_bid() << "\n";

    return 0;
}