#pragma once

#include <arpa/inet.h>
#include <algorithm>
#include <chrono>
#include <fstream>

#include "orderbook/OrderBook.hpp"

using Price4 = std::uint32_t;

namespace Messages {
    #pragma pack(push, 1)

        struct S_SystemEvent {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char event_code;
        };

        struct R_StockDirectory {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            char market_category;
            char financial_status;
            std::uint32_t round_lot_size;
            char round_lots_only;
            char issue_clarification;
            char issue_subtype[2];
            char authenticity;
            char short_sale_threshold_indicator;
            char ipo_flag;
            char LULD_tier;
            char ETP_flag;
            std::uint32_t ETP_leverage_factor;
            char inverse_indicator;
        };

        struct H_StockTradingAction {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            char trading_state;
            char reserved;
            char reason[4];
        };

        struct Y_RegSHOShortPriceTestRestricted {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            char RegSHOAction;
        };

        struct L_MarketParticipantPositon {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char MPID[4];
            char stock[8];
            char primary_maker;
            char maker_mode;
            char participant_state;
        };

        struct V_MarketWideCircuitBreakerDecline {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t level_1;
            std::uint64_t level_2;
            std::uint64_t level_3;
        };

        struct W_MarketWideCircuitBreakerStatus {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char breached_level;
        };

        struct K_IPOQuotingPeriodUpdate {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            std::uint32_t release_time;
            char release_qualifier;
            Price4 price;
        };

        struct J_LULDCollar {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            Price4 collar_ref_price;
            Price4 upper_collar_price;
            Price4 lower_collar_price;
            std::uint32_t collar_extension;
        };

        struct h_OperationalHalt {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            char market_code;
            char halt_action;
        };

        struct A_AddOrder {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t order_reference;
            char side;
            std::uint32_t shares;
            char stock[8];
            Price4 price;
        };

        struct F_AddOrderMPID {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t order_reference;
            char side;
            std::uint32_t shares;
            char stock[8];
            Price4 price;
            char attribution[4];
        };

        struct E_OrderExecuted {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t order_reference;
            std::uint32_t executed_shares;
            std::uint64_t match_number;
        };

        struct C_OrderExecutedMessage {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t order_reference;
            std::uint32_t executed_shares;
            std::uint64_t match_number;
            char printable;
            Price4 exectution_price;
        };

        struct X_OrderCancel {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t reference_number;
            std::uint32_t shares_cancelled;
        };

        struct D_OrderDelete {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t reference_number;
        };

        struct U_OrderReplace {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t original_reference;
            std::uint64_t new_reference;
            std::uint32_t shares;
            Price4 price;
        };

        struct P_TradeNonCross {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t reference_number;
            char side;
            std::uint32_t shares;
            char stock[8];
            Price4 price;
            std::uint64_t match_number;
        };

        struct Q_TradeCross {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t shares;
            char stock[8];
            Price4 cross_price;
            std::uint64_t match_number;
            char cross_type;
        };

        struct B_BrokenTrade {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t match_number;
        };

        struct I_NOII {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            std::uint64_t paired_shares;
            std::uint64_t imbalance_shares;
            char direction;
            char stock[8];
            Price4 far_price;
            Price4 near_price;
            Price4 current_ref_price;
            char cross_type;
            char price_var;
        };

        struct O_DirectListingCapitalRaise {
            std::uint16_t stock_locate;
            std::uint16_t tracking_number;
            char timestamp[6];
            char stock[8];
            char open_status;
            Price4 min_price;
            Price4 max_price;
            Price4 near_price;
            std::uint64_t near_time;
            Price4 lower_price_collar;
            Price4 upper_price_collar;
        };

    #pragma pack(pop)
} // namespace Messages

class ItchParser {
    public:
        ItchParser() = default; 

        void parse_file();

    private:
        void parse_message(char* buffer, size_t size, char message_type);

};