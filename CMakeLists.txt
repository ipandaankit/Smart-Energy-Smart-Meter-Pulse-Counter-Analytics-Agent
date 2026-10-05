cmake_minimum_required(VERSION 3.16)
project(smart_meter_agent VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(smart_meter_core
    src/config.cpp
    src/meter.cpp
    src/analyzer.cpp
    src/telemetry.cpp
)

target_include_directories(smart_meter_core PUBLIC include)

add_executable(smart_meter_agent src/main.cpp)
target_link_libraries(smart_meter_agent PRIVATE smart_meter_core)

enable_testing()
add_executable(smart_meter_tests tests/test_main.cpp)
target_link_libraries(smart_meter_tests PRIVATE smart_meter_core)
add_test(NAME smart_meter_tests COMMAND smart_meter_tests)

install(TARGETS smart_meter_agent DESTINATION bin)
