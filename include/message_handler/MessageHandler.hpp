#pragma once

#include <unordered_map>

#include "itch_parser/Messages.hpp"

class MessageHandler {
    public:
        std::unordered_map<uint16_t, std::size_t> stock_count_;
        
        std::uint64_t bswap64(std::uint64_t n) { return __builtin_bswap64(n); }
        std::uint32_t bswap32(std::uint32_t n) { return __builtin_bswap32(n); }
        std::uint16_t bswap16(std::uint16_t n) { return __builtin_bswap16(n); }

        virtual ~MessageHandler() = default;
        
        void handle_message(const ItchMessage& msg) {
            std::visit([&](auto&& m){ 
                stock_count_[bswap16(m.stock_locate)] += 1;
                process_message(m); 
            }, msg);
        };

    private:
        virtual void process_message(const Messages::S_SystemEvent&) {}
        virtual void process_message(const Messages::R_StockDirectory&) {}
        virtual void process_message(const Messages::H_StockTradingAction&) {}
        virtual void process_message(const Messages::Y_RegSHOShortPriceTestRestricted&) {}
        virtual void process_message(const Messages::L_MarketParticipantPositon&) {}
        virtual void process_message(const Messages::V_MarketWideCircuitBreakerDecline&) {}
        virtual void process_message(const Messages::W_MarketWideCircuitBreakerStatus&) {}
        virtual void process_message(const Messages::K_IPOQuotingPeriodUpdate&) {}
        virtual void process_message(const Messages::J_LULDCollar&) {}
        virtual void process_message(const Messages::h_OperationalHalt&) {}
        virtual void process_message(const Messages::A_AddOrder&) {}
        virtual void process_message(const Messages::F_AddOrderMPID&) {}
        virtual void process_message(const Messages::E_OrderExecuted&) {}
        virtual void process_message(const Messages::C_OrderExecutedMessage&) {}
        virtual void process_message(const Messages::X_OrderCancel&) {}
        virtual void process_message(const Messages::D_OrderDelete&) {}
        virtual void process_message(const Messages::U_OrderReplace&) {}
        virtual void process_message(const Messages::P_TradeNonCross&) {}
        virtual void process_message(const Messages::Q_TradeCross&) {}
        virtual void process_message(const Messages::B_BrokenTrade&) {}
        virtual void process_message(const Messages::I_NOII&) {}
        virtual void process_message(const Messages::O_DirectListingCapitalRaise&) {}
}; 