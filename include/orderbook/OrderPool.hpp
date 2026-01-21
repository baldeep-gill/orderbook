#pragma once

#include <memory>
#include <vector>

#include "Order.hpp"

class OrderPool {
    private:
        static constexpr size_t CAPACITY = 250'000;
        std::vector<std::unique_ptr<Order>> pointers_;
        std::vector<Order*> free_orders_;

    public:
        OrderPool() : pointers_{}, free_orders_{} {
            pointers_.reserve(CAPACITY);
        }

        Order* allocate() {
            if (!free_orders_.empty()) {
                Order* order = free_orders_.back();
                free_orders_.pop_back();

                return order;
            }

            std::unique_ptr<Order> ptr = std::make_unique<Order>();
            
            Order* order = ptr.get();
            pointers_.push_back(std::move(ptr));

            return order;
        }

        void free(Order* order) {
            free_orders_.push_back(order);
        }
};