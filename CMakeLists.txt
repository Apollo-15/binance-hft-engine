
cmake_minimum_required(VERSION 3.20)

project(binance_hft LANGUAGES CXX)

# ==================================================
# C++ Standard
# ==================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================================================
# Source files
# ==================================================

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/third_party/simdjson/src/*.cpp"
)

add_executable(binance_hft ${PROJECT_SOURCES})

# ==================================================
# Include directories
# ==================================================

target_include_directories(binance_hft PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/third_party/simdjson/include"
)

# ==================================================
# Dependencies
# ==================================================

find_package(OpenSSL REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Boost REQUIRED)

# ==================================================
# Link libraries
# ==================================================

target_link_libraries(binance_hft PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto

        fmt::fmt

        ftxui::component
        ftxui::dom
        ftxui::screen

        SQLite::SQLite3
)

# ==================================================
# Windows configuration
# ==================================================

if(WIN32)
    target_compile_definitions(binance_hft PRIVATE
            _WIN32_WINNT=0x0A00
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    # Windows ICMP API: IcmpCreateFile, IcmpCloseHandle,
    # IcmpSendEcho
    target_link_libraries(binance_hft PRIVATE
            Iphlpapi
    )
endif()

# ==================================================
# Compiler options
# ==================================================

if(MSVC)
    target_compile_options(binance_hft PRIVATE
            /W4
            /utf-8
            /bigobj
    )
else()
    target_compile_options(binance_hft PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

# ==================================================
# Copy private configuration
# ==================================================

set(CONFIG_SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/binance_hft/include/config/config_private.json"
)

if(EXISTS "${CONFIG_SOURCE}")
    configure_file(
            "${CONFIG_SOURCE}"
            "${CMAKE_CURRENT_BINARY_DIR}/include/config/config_private.json"
            COPYONLY
    )
else()
    message(WARNING
            "config_private.json not found: ${CONFIG_SOURCE}"
    )
endif()
