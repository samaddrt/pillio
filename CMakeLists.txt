cmake_minimum_required(VERSION 3.19)
project(pillio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    if(MINGW)
        add_compile_options(-Wa,-mbig-obj)
    endif()
endif()

# ── Dependencies via FetchContent ───────────────────────────────────
include(FetchContent)

FetchContent_Declare(httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.15.3
)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.11
)

FetchContent_MakeAvailable(httplib nlohmann_json doctest)

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

if(UNIX)
    target_link_libraries(pillio PRIVATE pthread)
endif()

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
