cmake_minimum_required(VERSION 3.20)
project(pillio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    # MinGW: httplib + regex generate huge debug objects
    if(MINGW)
        add_compile_options(-Wa,-mbig-obj)
    endif()
endif()

# ── Dependencies via vcpkg ──────────────────────────────────────────
find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

# ── Main executable ─────────────────────────────────────────────────
add_executable(pillio
    src/main.cpp
    src/tracker.cpp
    src/storage.cpp
    src/analytics.cpp
    src/checker.cpp
    src/family.cpp
)

target_include_directories(pillio PRIVATE src)
target_link_libraries(pillio PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)

# ── Test executable ─────────────────────────────────────────────────
add_executable(pillio_tests
    tests/tests.cpp
    src/tracker.cpp
    src/storage.cpp
    src/analytics.cpp
    src/checker.cpp
    src/family.cpp
)

target_include_directories(pillio_tests PRIVATE src)
target_link_libraries(pillio_tests PRIVATE
    nlohmann_json::nlohmann_json
    doctest::doctest
)

enable_testing()
add_test(NAME pillio_tests COMMAND pillio_tests)
