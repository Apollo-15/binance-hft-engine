cmake_minimum_required(VERSION 3.20)

project(binance_hft)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/third_party/simdjson/src/*.cpp"
)

add_executable(binance_hft ${SOURCES}
        binance_hft/src/logger/logger_manager.cpp
)

target_include_directories(binance_hft PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/third_party/simdjson/include"
)


find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
find_package(ftxui REQUIRED)

target_link_libraries(binance_hft PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        sqlite3
        fmt::fmt
        ftxui::component
        ftxui::dom
        ftxui::screen
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/include/config/config_private.json"
        "${CMAKE_CURRENT_BINARY_DIR}/include/config/config_private.json"
        COPYONLY
)