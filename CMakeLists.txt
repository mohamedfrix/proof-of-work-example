cmake_minimum_required(VERSION 3.14)
project(cns-blockchain
    VERSION 0.0.1
    DESCRIPTION "CNS Blockchain Primary Project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
)

add_executable(cns-blockchain ${SOURCES})
target_link_libraries(cns-blockchain
    PRIVATE
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)
