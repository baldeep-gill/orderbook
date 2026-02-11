add_executable(
    bench_all
    bench_orderbook.cpp
    bench_itchparser.cpp
    bench_orderpool.cpp
    bench_matching.cpp
)

target_link_libraries(bench_all PRIVATE
    orderbook
    itch_parser
    orderbook_message_handler
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(bench_all PRIVATE ../include)

target_compile_features(bench_all PUBLIC cxx_std_20)

##################################################################

add_executable(
    bench_itch
    bench_itchparser.cpp
)

target_link_libraries(bench_itch PRIVATE 
    orderbook
    itch_parser
    orderbook_message_handler
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(bench_itch PRIVATE ../include)

target_compile_features(bench_itch PUBLIC cxx_std_20)

##################################################################

add_executable(
    bench_orderbook
    bench_orderbook.cpp
    bench_orderpool.cpp
)

target_link_libraries(bench_orderbook PRIVATE 
    orderbook
    itch_parser
    orderbook_message_handler
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(bench_orderbook PRIVATE ../include)

target_compile_features(bench_orderbook PUBLIC cxx_std_20)

##################################################################

add_executable(
    bench_matching
    bench_matching.cpp
)

target_link_libraries(bench_matching PRIVATE 
    orderbook
    itch_parser
    orderbook_message_handler
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(bench_matching PRIVATE ../include)

target_compile_features(bench_matching PUBLIC cxx_std_20)