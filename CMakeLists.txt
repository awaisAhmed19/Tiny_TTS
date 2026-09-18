cmake_minimum_required(VERSION 3.21)

project(
    TinyTTS
    VERSION 1.0.0
    LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# Options
# -----------------------------------------------------------------------------

option(
    TINY_TTS_USE_FFTW
    "Enable FFTW for FFT validation/benchmarking"
    OFF
)

option(
    TINY_TTS_USE_STB_IMAGE_WRITE
    "Enable stb_image_write for image output"
    OFF
)

option(
    TINY_TTS_USE_PHONETISAURUS
    "Enable Phonetisaurus for G2P validation"
    OFF
)

# -----------------------------------------------------------------------------
# General configuration
# -----------------------------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
    TINY_TTS_VENDOR_DIR
    "${CMAKE_SOURCE_DIR}/vendor"
)

# -----------------------------------------------------------------------------
# TinyTTS
# -----------------------------------------------------------------------------

file(
    GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(
    tinytts
    ${SOURCES}
)

target_compile_features(
    tinytts
    PRIVATE
        cxx_std_20
)

target_include_directories(
    tinytts
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
)

# -----------------------------------------------------------------------------
# Compiler warnings
# -----------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        tinytts
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wno-unused-parameter
    )
endif()

if(MSVC)
    target_compile_options(
        tinytts
        PRIVATE
            /W4
            /permissive-
    )
endif()

# -----------------------------------------------------------------------------
# Optional validation dependencies
# -----------------------------------------------------------------------------

if(TINY_TTS_USE_STB_IMAGE_WRITE)

    add_library(
        stb_image_write
        INTERFACE
    )

    target_include_directories(
        stb_image_write
        INTERFACE
            "${TINY_TTS_VENDOR_DIR}"
    )

endif()

if(TINY_TTS_USE_FFTW)

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(
        FFTW3
        REQUIRED
        IMPORTED_TARGET
        fftw3
    )

    add_library(
        tinytts_fftw
        INTERFACE
    )

    target_link_libraries(
        tinytts_fftw
        INTERFACE
            PkgConfig::FFTW3
    )

endif()

if(TINY_TTS_USE_PHONETISAURUS)

    find_program(
        PHONETISAURUS_G2P
        NAMES phonetisaurus-g2pfst
        REQUIRED
    )

    message(
        STATUS
        "Phonetisaurus G2P: ${PHONETISAURUS_G2P}"
    )

endif()

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

include(CTest)

add_executable(
    test_correctness
    tests/test_test.cpp
)

add_test(
    NAME correctness
    COMMAND test_correctness
)

# -----------------------------------------------------------------------------
# Configuration summary
# -----------------------------------------------------------------------------

message(STATUS "")
message(STATUS "  ┌─────────────────────────────────────────┐")
message(STATUS "  │  TinyTTS ${PROJECT_VERSION} — ${CMAKE_BUILD_TYPE}")
message(STATUS "  │")
message(
    STATUS
    "  │  Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
)
message(
    STATUS
    "  │  Build dir  : ${CMAKE_BINARY_DIR}"
)
message(STATUS "  └─────────────────────────────────────────┘")
message(STATUS "")
