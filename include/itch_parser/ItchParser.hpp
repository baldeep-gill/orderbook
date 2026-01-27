#pragma once

#include <arpa/inet.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <variant>

#include "Messages.hpp"
#include "orderbook/OrderBook.hpp"

using ItchMessage = std::variant<
    Messages::S_SystemEvent, 
    Messages::R_StockDirectory,
    Messages::H_StockTradingAction,
    Messages::Y_RegSHOShortPriceTestRestricted,
    Messages::L_MarketParticipantPositon,
    Messages::V_MarketWideCircuitBreakerDecline,
    Messages::W_MarketWideCircuitBreakerStatus,
    Messages::K_IPOQuotingPeriodUpdate,
    Messages::J_LULDCollar,
    Messages::h_OperationalHalt,
    Messages::A_AddOrder,
    Messages::F_AddOrderMPID,
    Messages::E_OrderExecuted,
    Messages::C_OrderExecutedMessage,
    Messages::X_OrderCancel,
    Messages::D_OrderDelete,
    Messages::U_OrderReplace,
    Messages::P_TradeNonCross,
    Messages::Q_TradeCross,
    Messages::B_BrokenTrade,
    Messages::I_NOII,
    Messages::O_DirectListingCapitalRaise
>;

using MessageCallback = std::function<void(const ItchMessage&)>;

class ItchParser {
    public:
        ItchParser(MessageCallback cb) : parser_callback_{cb} {}; 

        void parse_file();

    private:
        MessageCallback parser_callback_;

        ItchMessage parse_message(char* buffer, size_t size, char message_type);
};