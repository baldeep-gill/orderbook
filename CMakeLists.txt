cmake_minimum_required(VERSION 3.25)
project(OrderBook VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY  https://github.com/google/benchmark.git
    GIT_TAG         v1.9.5
)
FetchContent_MakeAvailable(googlebenchmark)

add_library(orderbook src/OrderBook.cpp)
target_include_directories(orderbook PUBLIC include)
target_compile_features(orderbook PUBLIC cxx_std_20)

add_library(itch_parser INTERFACE)
target_include_directories(itch_parser INTERFACE include/itch_parser)

add_library(orderbook_message_handler src/OrderBookMessageHandler.cpp)
target_include_directories(orderbook_message_handler PUBLIC include)
target_compile_features(orderbook_message_handler PUBLIC cxx_std_20)

add_library(test_message_handler src/TestMessageHandler.cpp)
target_include_directories(test_message_handler PUBLIC include)
target_compile_features(test_message_handler PUBLIC cxx_std_20)

add_executable(orderbook_app main.cpp)
target_link_libraries(orderbook_app PRIVATE orderbook)
target_link_libraries(orderbook_app PRIVATE itch_parser)
target_link_libraries(orderbook_app PRIVATE orderbook_message_handler)
target_link_libraries(orderbook_app PRIVATE test_message_handler)

add_subdirectory(tests)
add_subdirectory(bench)