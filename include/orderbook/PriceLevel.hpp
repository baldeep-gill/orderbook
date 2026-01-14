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

        Quantity match(Quantity incoming) {
            Quantity total = 0;

            while (!level.empty() && incoming > 0) {
                Order* order = level.front();

                Quantity filled = std::min(incoming, order->quantity);
                order->quantity -= filled;
                total += filled;
                incoming -= filled;
                
                if (order->quantity == 0) {
                    level.pop_front();
                }
            }

            quantity -= total;

            return total;
        }

        bool empty() { return level.empty (); }
    
    private:
        std::list<Order*> level{};
        Quantity quantity{0};
};