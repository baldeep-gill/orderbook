#pragma once

#include "Order.hpp"
#include "OrderPool.hpp"

struct IntrusivePriceLevel {
    private: 
        Quantity quantity_{0};
        Order* head_{nullptr};
        Order* tail_{nullptr};
        size_t length_{0};

    public:
        void insert(Order* order) {       
            if (tail_) {
                tail_->next = order;
                order->prev = tail_;
                tail_ = order;
            }

            if (!head_) {
                head_ = order;
                tail_ = order;
            }

            quantity_ += order->open_quantity();
            ++length_;
        }

        void erase(Order* order) {
            Order* prev_node = order->prev;
            Order* next_node = order->next;
            
            if (prev_node) prev_node->next = next_node;
            else head_ = next_node;

            if (next_node) next_node->prev = prev_node;
            else tail_ = prev_node;
            
            quantity_ -= order->open_quantity();
            --length_;
        }

        Quantity quantity() const {
            return quantity_;
        }

        bool empty() const {
            return length_ == 0;
        }

        size_t length() const {
            return length_;
        }
        
        Quantity match(Quantity incoming, OrderPool& pool) {
            Quantity total = 0;

            while (head_ && incoming > 0) {
                Quantity filled = std::min(incoming, head_->open_quantity());

                head_->filled_quantity += filled;

                total += filled;
                incoming -= filled;

                if (head_->open_quantity() == 0) {
                    erase(head_);
                    pool.free(head_);
                }
            }

            quantity_ -= total;
            return total;
        }
};