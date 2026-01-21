#pragma once

#include <memory>
#include <vector>

#include "Order.hpp"

class OrderPool {
    private:
        static constexpr size_t CAPACITY = 250'000;
        static constexpr size_t BLOCK_SIZE = 50'000;
        
        std::vector<std::unique_ptr<Order[]>> blocks_;
        std::size_t BLOCK_POINTER = 0;
        std::vector<Order*> free_orders_;

    public:
        OrderPool() : free_orders_{} {
            blocks_.emplace_back(std::make_unique<Order[]>(CAPACITY));
            free_orders_.reserve(CAPACITY / 4);
        }

        ~OrderPool() = default;

        Order* allocate() {
            if (!free_orders_.empty()) {
                Order* order = free_orders_.back();
                free_orders_.pop_back();

                return order;
            }
            
            if (blocks_.empty() || BLOCK_POINTER == BLOCK_SIZE) {
                blocks_.emplace_back(std::make_unique<Order[]>(BLOCK_SIZE));
                BLOCK_POINTER = 0;
            }

            auto block = blocks_.back().get();
            Order* order = &block[BLOCK_POINTER];

            return order;
        }

        void free(Order* order) {
            free_orders_.push_back(order);
        }
};