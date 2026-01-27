#pragma once

#include <arpa/inet.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>

#include "Messages.hpp"
#include "orderbook/OrderBook.hpp"

using MessageCallback = std::function<void(const ItchMessage&)>;

class ItchParser {
    public:
        ItchParser(MessageCallback cb) : parser_callback_{cb} {}; 

        void parse_file();

    private:
        MessageCallback parser_callback_;

        ItchMessage parse_message(char* buffer, size_t size, char message_type);
};