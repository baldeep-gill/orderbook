#include "itch_parser/ItchParser.hpp"

void ItchParser::parse_file() {
    std::ifstream file("../data/S071321-v50.bin", std::ios::binary);

    std::uint16_t message_size;
    size_t count{0};

    while (file.read(reinterpret_cast<char*>(&message_size), 2)) {
        message_size = ntohs(message_size);

        char message_type;
        file.read(&message_type, 1);

        char buffer[message_size - 1];
        file.read(buffer, message_size - 1);

        parse_message(buffer, message_size - 1, message_type);
        ++count;
    }
}

void ItchParser::parse_message(char* buffer, size_t size, char message_type) {
    switch (message_type) {
        case 'S': {
            Messages::S_SystemEvent msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'R': {
            Messages::R_StockDirectory msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'H': {
            Messages::H_StockTradingAction msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'Y': {
            Messages::Y_RegSHOShortPriceTestRestricted msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'L': {
            Messages::L_MarketParticipantPositon msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'V': {
            Messages::V_MarketWideCircuitBreakerDecline msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'W': {
            Messages::W_MarketWideCircuitBreakerStatus msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'K': {
            Messages::K_IPOQuotingPeriodUpdate msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'J': {
            Messages::J_LULDCollar msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'h': {
            Messages::h_OperationalHalt msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'A': {
            Messages::A_AddOrder msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'F': {
            Messages::F_AddOrderMPID msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'E': {
            Messages::E_OrderExecuted msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'C': {
            Messages::C_OrderExecutedMessage msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'X': {
            Messages::X_OrderCancel msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'D': {
            Messages::D_OrderDelete msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'U': {
            Messages::U_OrderReplace msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'P': {
            Messages::P_TradeNonCross msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'Q': {
            Messages::Q_TradeCross msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'B': {
            Messages::B_BrokenTrade msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'I': {
            Messages::I_NOII msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        case 'O': {
            Messages::O_DirectListingCapitalRaise msg;
            std::copy(buffer, buffer + size, reinterpret_cast<char*>(&msg));
            break;
        }
        default: {
            throw std::runtime_error("No matching message_type found!");
        }
    }
}