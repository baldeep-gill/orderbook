#pragma once

#include <string>

#include "MessageHandler.hpp"

class TestMessageHandler : public MessageHandler {
    public:
        explicit TestMessageHandler() {
            collect_debug_ = true;
        };
        ~TestMessageHandler() = default;

        void handle_message(const ItchMessage&);

        const std::string get_stock(std::uint16_t locate) const;
        const std::uint16_t get_locate(const std::string&) const;
        void print_counts() const;
        void print_locates() const;
};