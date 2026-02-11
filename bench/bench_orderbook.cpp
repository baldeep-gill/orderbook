#include <random>
#include <vector>

#include <benchmark/benchmark.h>
#include "orderbook/OrderBook.hpp"

struct Command {
    enum class Type { Add, Cancel } type;
    std::uint64_t id;
    Side side;
    Price price;
    Quantity quantity;
};

std::vector<Command> generate_work(size_t n_adds, size_t n_cancels) {
    std::vector<Command> commands{};
    commands.reserve(n_adds + n_cancels);

    std::mt19937_64 rng{1738};
    std::uniform_real_distribution<double> price_distribution{99.0, 101.0};
    std::uniform_int_distribution<std::int64_t> qty_distribution{1, 1000};

    for (size_t i = 0; i < n_adds; ++i) {
        Command c{
            .type = Command::Type::Add,
            .id = static_cast<std::uint64_t>(i + 1),
            .side = static_cast<Side>(i % 2 == 0),
            .price = price_distribution(rng),
            .quantity = qty_distribution(rng)
        };
        commands.push_back(c);
    }

    std::uniform_int_distribution<size_t> id_idx{0, n_adds - 1};
    for (size_t i = 0; i < n_cancels; ++i) {
        auto idx = id_idx(rng);
        commands.push_back({
            .type = Command::Type::Cancel, 
            .id = commands[idx].id, 
            .quantity = commands[idx].quantity
        });
    }

    return commands;
}

static void BM_OrderBook_AddAndCancel(benchmark::State& state) {
    const size_t n_adds = static_cast<size_t>(state.range(0));
    const size_t n_cancels = static_cast<size_t>(state.range(1));

    auto commands = generate_work(n_adds, n_cancels);

    for (auto _ : state) {
        OrderBook book{};
        benchmark::DoNotOptimize(&book);

        for (const auto& c: commands) {
            if (c.type == Command::Type::Add) {
                book.add_order(c.id, c.side, c.price, c.quantity);
            } else {
                book.cancel_order(c.id, c.quantity);
            }
        }

        state.PauseTiming();
    }
    
    state.SetComplexityN(n_adds + n_cancels);
    state.SetLabel("adds-and-cancels");
}

BENCHMARK(BM_OrderBook_AddAndCancel)
    ->Unit(benchmark::kMillisecond)
    ->Ranges({ {50'000, 100'000}, {250'000, 500'000} })
    ->Apply([](::benchmark::Benchmark* b) {
        b->Complexity(benchmark::oN);
    });