#pragma once

#include <arpa/inet.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>

#include "Messages.hpp"
#include "orderbook/OrderBook.hpp"
#include "message_handler/MessageHandler.hpp"

using MessageCallback = std::function<void(const ItchMessage&)>;

class ItchParser {
    public:
        ItchParser(std::unique_ptr<MessageHandler> handle) : message_handler_{std::move(handle)} {};

        void parse_file();

    private:
        std::unique_ptr<MessageHandler> message_handler_;

        ItchMessage parse_message(char* buffer, size_t size, char message_type);
};