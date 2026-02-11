#include "orderbook/OrderPool.hpp"
#include "orderbook/PriceLevel.hpp"
#include <benchmark/benchmark.h>

static void BM_AddOrdersSameLevel(benchmark::State& state) {
    size_t num_orders = 1e7;
    OrderPool pool(num_orders);
    PriceLevel level;

    for (auto _: state) {
        for (size_t i = 0; i < num_orders; ++i) {
            Order* o = pool.allocate();
            benchmark::DoNotOptimize(o);
            level.insert(o);
        }
        state.PauseTiming();
        while (level.head()) {
            level.erase(level.head(), pool);
        }

        state.ResumeTiming();
    }
    state.SetItemsProcessed(state.iterations() * num_orders);
}

BENCHMARK(BM_AddOrdersSameLevel)->Unit(benchmark::kMillisecond);