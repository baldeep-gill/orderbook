cmake_minimum_required(VERSION 3.25)
project(OrderBook VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -Wall -Wextra")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -fsanitize=address -fsanitize=undefined")

enable_testing()

add_library(orderbook src/OrderBook.cpp)
target_include_directories(orderbook PUBLIC include)
target_compile_features(orderbook PUBLIC cxx_std_20)

add_library(itch_parser src/ItchParser.cpp)
target_include_directories(itch_parser PUBLIC include)
target_compile_features(itch_parser PUBLIC cxx_std_20)

add_library(message_handler src/MessageHandler.cpp)
target_include_directories(message_handler PUBLIC include)
target_compile_features(message_handler PUBLIC cxx_std_20)

add_executable(orderbook_app main.cpp)
target_link_libraries(orderbook_app PRIVATE orderbook)
target_link_libraries(orderbook_app PRIVATE itch_parser)
target_link_libraries(orderbook_app PRIVATE message_handler)

add_subdirectory(tests)