#pragma once

#include <arpa/inet.h>
#include <algorithm>
#include <fcntl.h>
#include <memory>
#include <fstream>
#include <netinet/in.h>
#include <sys/mman.h>
#include <unistd.h>

#include "Messages.hpp"

template<typename Handler>
class ItchParser {
    public:
        explicit ItchParser(std::unique_ptr<Handler> handle) : message_handler_{std::move(handle)} {};

        void parse_file() {
            std::ifstream file("../data/S071321-v50.bin", std::ios::binary);

            std::uint16_t message_size;

            while (file.read(reinterpret_cast<char*>(&message_size), 2)) {
                message_size = ntohs(message_size);

                char message_type;
                file.read(&message_type, 1);

                char buffer[message_size];
                buffer[0] = message_type;

                file.read(buffer + 1, message_size - 1);

                ItchMessage msg = parse_message(buffer, message_size, message_type);
                message_handler_->handle_message(msg);
            }
        }

        void mmap_parse() {
            int fd = open("../data/S071321-v50.bin", O_RDONLY);
            size_t size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            char* mapped = static_cast<char*>(
                mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0)
            );

            for (size_t offset = 0; offset + 2 <= size; ) {
                std::uint16_t message_size = ntohs(*reinterpret_cast<const std::uint16_t*>(&mapped[offset]));
                offset += 2;

                if (offset + message_size > size) break;

                char* msg = &mapped[offset];
                offset += message_size;

                ItchMessage itch_msg = parse_message(msg, message_size, msg[0]);
                message_handler_->handle_message(itch_msg);
            }

            munmap(mapped, size);
            close(fd);
        }

        const Handler& get_handler() const { return *message_handler_; }

    protected:
        std::unique_ptr<Handler> message_handler_;

        ItchMessage parse_message(char* buffer, size_t size, char message_type) {
            switch (message_type) {
                case 'S': {
                    Messages::S_SystemEvent msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'R': {
                    Messages::R_StockDirectory msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'H': {
                    Messages::H_StockTradingAction msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'Y': {
                    Messages::Y_RegSHOShortPriceTestRestricted msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'L': {
                    Messages::L_MarketParticipantPositon msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'V': {
                    Messages::V_MarketWideCircuitBreakerDecline msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'W': {
                    Messages::W_MarketWideCircuitBreakerStatus msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'K': {
                    Messages::K_IPOQuotingPeriodUpdate msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'J': {
                    Messages::J_LULDCollar msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'h': {
                    Messages::h_OperationalHalt msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'A': {
                    Messages::A_AddOrder msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'F': {
                    Messages::F_AddOrderMPID msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'E': {
                    Messages::E_OrderExecuted msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'C': {
                    Messages::C_OrderExecutedMessage msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'X': {
                    Messages::X_OrderCancel msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'D': {
                    Messages::D_OrderDelete msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'U': {
                    Messages::U_OrderReplace msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'P': {
                    Messages::P_TradeNonCross msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'Q': {
                    Messages::Q_TradeCross msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'B': {
                    Messages::B_BrokenTrade msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'I': {
                    Messages::I_NOII msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                case 'O': {
                    Messages::O_DirectListingCapitalRaise msg;
                    std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
                    return msg;
                }
                default: {
                    throw std::runtime_error("No matching message_type found!");
                }
            }
        }
};