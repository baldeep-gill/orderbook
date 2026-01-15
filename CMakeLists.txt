cmake_minimum_required(VERSION 4.2)
project(OrderBook VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(orderbook src/OrderBook.cpp)
target_include_directories(orderbook PUBLIC include)
target_compile_features(orderbook PUBLIC cxx_std_20)


add_executable(orderbook_app main.cpp)
target_link_libraries(orderbook_app PRIVATE orderbook)