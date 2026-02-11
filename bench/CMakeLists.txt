add_executable(
    orderbook_bench
    bench_orderbook.cpp
    bench_itchparser.cpp
)

target_link_libraries(orderbook_bench PRIVATE
    orderbook
    itch_parser
    orderbook_message_handler
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(orderbook_bench PRIVATE ../include)

target_compile_features(orderbook_bench PUBLIC cxx_std_20)

add_test(NAME orderbook_bench COMMAND orderbook_bench)