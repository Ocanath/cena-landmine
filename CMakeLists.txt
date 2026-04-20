cmake_minimum_required(VERSION 3.22)
project(cena_landmine C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# keylogger — platform-specific static library, no external dependencies
add_library(keylogger STATIC src/keylogger.cpp)
target_include_directories(keylogger PUBLIC src)

if(APPLE)
    target_link_libraries(keylogger PUBLIC "-framework ApplicationServices")
elseif(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(keylogger PUBLIC Threads::Threads)
elseif(WIN32)
    target_link_libraries(keylogger PUBLIC user32)
endif()

add_executable(cena_main src/main.cpp)
target_link_libraries(cena_main PRIVATE keylogger)
