include(FetchContent)

add_executable(
    orderbook_tests
    test_orderbook.cpp
    test_order.cpp
    test_pricelevel.cpp
)

target_link_libraries(orderbook_tests
    PRIVATE
    gtest_main
    orderbook    
)

target_include_directories(orderbook_tests PRIVATE ../include)

include(GoogleTest)
gtest_discover_tests(orderbook_tests)