#include "itch_parser/ItchParser.hpp"

void ItchParser::parse_file() {
    std::ifstream file("../data/S071321-v50.bin", std::ios::binary);

    std::uint16_t message_size;
    size_t count{0};

    while (file.read(reinterpret_cast<char*>(&message_size), 2)) {
        message_size = ntohs(message_size);

        char message_type;
        file.read(&message_type, 1);

        char buffer[message_size];
        buffer[0] = message_type;

        file.read(buffer + 1, message_size - 1);

        ItchMessage msg = parse_message(buffer, message_size, message_type);
        message_handler_->handle_message(msg);
        
        ++count;
    }
}

ItchMessage ItchParser::parse_message(char* buffer, size_t size, char message_type) {
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