#include <cstdint>
#include <cstdlib>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>
#include "orderbook/OrderBook.hpp"

struct Scenario {
    enum Operator { AddBuy, AddSell, MarketBuy, MarketSell, Cancel } op;
    std::uint64_t id;
    Price price;
    Quantity qty;
};

std::vector<Scenario> generate_work(size_t N, double prob = 0.4) {
    std::mt19937_64 rng{1738};
    std::uniform_real_distribution<double> price{100.0, 101.0};
    std::uniform_int_distribution<std::int64_t> qty{1, 100};

    std::vector<Scenario> scenario;
    scenario.reserve(N);

    Price spread = 0.01;

    for (size_t i = 0; i < N; ++i) {
        if (rng() / double(RAND_MAX) < prob) {
            scenario.push_back({
                rng() % 2 ? Scenario::Operator::MarketBuy : Scenario::Operator::MarketSell,
                i + 1,
                0.0,
                qty(rng)
            });
        } else {
            bool is_buy = rng() % 2;
            double offset = (rng() / double(RAND_MAX)) * spread * 0.9;
            Price p = is_buy ? 100.0 + offset : 101.0 - offset;

            scenario.push_back({
                is_buy ? Scenario::Operator::AddBuy : Scenario::Operator::AddSell,
                i + 1,
                p,
                qty(rng)
            });
        }
    }

    return scenario;
}

static void BM_OrderBook_BenchMatches(benchmark::State& state) {
    size_t ops = state.range(0);
    double ratio = state.range(1) / 10.0;

    auto work = generate_work(ops * 10, ratio);

    for (auto _ : state) {
        OrderBook book{};
        benchmark::DoNotOptimize(book);

        for (const auto& cmd: work) {
            switch (cmd.op) {
                case Scenario::AddBuy:
                    book.add_order(cmd.id, Side::Buy, cmd.price, cmd.qty);
                    break;
                case Scenario::AddSell:
                    book.add_order(cmd.id, Side::Sell, cmd.price, cmd.price);
                    break;
                case Scenario::MarketBuy:
                    book.market_order(Side::Buy, cmd.qty);
                    break;
                case Scenario::MarketSell:
                    book.market_order(Side::Sell, cmd.qty);
                    break;
                case Scenario::Cancel:
                    book.delete_order(cmd.id);
                    break;
            }
        }
    }

    state.SetItemsProcessed(state.iterations() * ops);
    state.SetComplexityN(ops);
    state.counters["cross ratio"] = ratio;
}

static void BM_OrderBook_MarketSweep(benchmark::State& state)
{
    size_t book_depth = state.range(0);  // # resting orders
    
    // Build deep book once
    auto workload = generate_work(book_depth, 0.0);  // all adds
    
    for (auto _ : state) {
        OrderBook book;
        
        // Fill book with resting liquidity
        for (const auto& cmd : workload) {
            if (cmd.op == Scenario::Operator::AddSell)
                book.add_order(cmd.id, Side::Sell, cmd.price, cmd.qty);
        }
        
        // Single massive market buy sweeps entire book
        benchmark::DoNotOptimize(book.market_order(Side::Buy, 999999999ULL));
    }
    
    state.SetItemsProcessed(book_depth);
}

BENCHMARK(BM_OrderBook_BenchMatches)
    ->Args({2'000'000, 4})
    ->Args({1'000'000, 3})
    ->Complexity()
    ->Unit(benchmark::kMillisecond);

BENCHMARK(BM_OrderBook_MarketSweep)
    ->Range(1'000'000, 5'000'000)
    ->Unit(benchmark::kMillisecond);