#pragma once

#include <memory>
#include <vector>

#include "Order.hpp"

class OrderPool {
    private:
        std::vector<std::unique_ptr<Order>> pointers_;
        std::vector<Order*> free_orders_;

    public:
        Order* allocate() {
            if (!free_orders_.empty()) {
                Order* order = free_orders_.back();
                free_orders_.pop_back();

                return order;
            }

            std::unique_ptr<Order> ptr = std::make_unique<Order>();
            pointers_.push_back(ptr);

            return ptr.get();
        }

        void free(Order* order) {
            free_orders_.push_back(order);
        }
};