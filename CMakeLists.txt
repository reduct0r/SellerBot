cmake_minimum_required(VERSION 3.10.2)
project(SellerBot)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
find_package(jsoncpp)

include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

set(SOURCES
    "main.cpp"
    "Product/Product.cpp"
    "Bot/Bot.cpp"
    "State/State.cpp"
    "States/MainMenu/MainMenuState.cpp"
    "States/Catalog/CatalogState.cpp"
    "States/Category/CategoryState.cpp"
 "States/CheckOut/CheckOutState.cpp")

set(HEADERS
    "Product/Product.h"
    "Bot/Bot.h"
    "State/State.h"
    "States/MainMenu/MainMenuState.h"
    "States/Catalog/CatalogState.h"
    "States/Category/CategoryState.h"
 "States/CheckOut/CheckOutState.h")

add_executable(SellerBot ${SOURCES} ${HEADERS})

target_link_libraries(
    SellerBot
    ${CMAKE_SOURCE_DIR}/third-party/TgBot.lib
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
)
target_link_libraries(SellerBot JsonCpp::JsonCpp)
