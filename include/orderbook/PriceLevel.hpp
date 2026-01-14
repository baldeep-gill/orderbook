#pragma once

#include "Order.hpp"

struct PriceLevel {
    public:
        using Iterator = std::list<Order*>::iterator;

        Iterator insert(Order* order) {
            level.push_back(order);
            quantity += order->quantity;
            return std::prev(level.end());
        }

        void erase(Iterator it) {
            quantity -= (*it)->quantity;
            level.erase(it);
        }
        
        Quantity get_quantity() const {
            return quantity; 
        }
    
    private:
        std::list<Order*> level{};
        Quantity quantity{0};
};