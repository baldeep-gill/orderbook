include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

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