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

# -----------------------------------------------------------------------
# miniaudio — header-only, expose as interface target
# -----------------------------------------------------------------------
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE external/miniaudio)

# -----------------------------------------------------------------------
# audio — wraps miniaudio + bundled WAV asset
# -----------------------------------------------------------------------
add_library(audio STATIC src/audio.cpp)
target_include_directories(audio PUBLIC src)
target_link_libraries(audio PUBLIC miniaudio)

if(APPLE)
    target_link_libraries(audio PUBLIC
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework CoreFoundation"
    )
elseif(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(audio PUBLIC Threads::Threads ${CMAKE_DL_LIBS} m)
elseif(WIN32)
    target_link_libraries(audio PUBLIC ole32 winmm)
endif()

# -----------------------------------------------------------------------
# volume_maximizer — platform-specific static library
# -----------------------------------------------------------------------
add_library(volume_maximizer STATIC src/volume_maximizer.cpp)
target_include_directories(volume_maximizer PUBLIC src)

if(WIN32)
    target_link_libraries(volume_maximizer PUBLIC ole32)
endif()

# -----------------------------------------------------------------------
# cena_main — entry point
# -----------------------------------------------------------------------
add_executable(cena_main src/main.cpp)
target_link_libraries(cena_main PRIVATE keylogger audio volume_maximizer)
