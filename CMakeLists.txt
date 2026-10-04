cmake_minimum_required(VERSION 3.21)

project(
  TinyTTS
  VERSION 1.0.0
  LANGUAGES CXX)

option(TINY_TTS_USE_CUDA "Enable CUDA backend" OFF)

option(TINY_TTS_USE_FFTW "Enable FFTW for FFT validation/benchmarking" OFF)

option(TINY_TTS_USE_STB_IMAGE_WRITE "Enable stb_image_write for image output"
       OFF)

option(TINY_TTS_USE_PHONETISAURUS "Enable Phonetisaurus for G2P validation" OFF)

if(TINY_TTS_USE_CUDA)

  include(CheckLanguage)

  check_language(CUDA)

  if(NOT CMAKE_CUDA_COMPILER)
    message(FATAL_ERROR "TINY_TTS_USE_CUDA=ON but no CUDA compiler was found.")
  endif()

  enable_language(CUDA)

  find_package(CUDAToolkit REQUIRED)

  set(CMAKE_CUDA_STANDARD 20)
  set(CMAKE_CUDA_STANDARD_REQUIRED ON)

  message(STATUS "CUDA compiler : ${CMAKE_CUDA_COMPILER}")

  message(STATUS "CUDA version  : ${CMAKE_CUDA_COMPILER_VERSION}")

endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug when no configuration was explicitly supplied.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Build type" FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release
                                             RelWithDebInfo MinSizeRel)

# ============================================================
# VENDOR
# ============================================================

set(TINY_TTS_VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

if(TINY_TTS_USE_CUDA)

  file(GLOB_RECURSE CUDA_SOURCES CONFIGURE_DEPENDS
       "${CMAKE_SOURCE_DIR}/src/*.cu")

  list(APPEND SOURCES ${CUDA_SOURCES})

endif()

add_executable(tinytts ${SOURCES})
target_compile_features(tinytts PRIVATE cxx_std_20)
target_include_directories(tinytts PRIVATE "${CMAKE_SOURCE_DIR}/src")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
    tinytts PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
                    -Wno-unused-parameter)

endif()

if(MSVC)
  target_compile_options(tinytts PRIVATE /W4 /permissive-)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(tinytts PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-O0>
                                         $<$<COMPILE_LANGUAGE:CXX>:-g>)
  if(TINY_TTS_USE_CUDA)
    target_compile_options(tinytts PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-G>
                                           $<$<COMPILE_LANGUAGE:CUDA>:-g>)
  endif()

endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(tinytts PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-O3>)
endif()

if(TINY_TTS_USE_CUDA)
  set_target_properties(tinytts PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
  target_link_libraries(tinytts PRIVATE CUDA::cudart)
endif()

if(TINY_TTS_USE_STB_IMAGE_WRITE)
  add_library(stb_image_write INTERFACE)
  target_include_directories(stb_image_write INTERFACE "${TINY_TTS_VENDOR_DIR}")
  target_link_libraries(tinytts PRIVATE stb_image_write)
endif()

if(TINY_TTS_USE_FFTW)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(FFTW3 REQUIRED IMPORTED_TARGET fftw3)
  add_library(tinytts_fftw INTERFACE)
  target_link_libraries(tinytts_fftw INTERFACE PkgConfig::FFTW3)
  target_link_libraries(tinytts PRIVATE tinytts_fftw)
endif()

if(TINY_TTS_USE_PHONETISAURUS)
  find_program(PHONETISAURUS_G2P NAMES phonetisaurus-g2pfst REQUIRED)
  message(STATUS "Phonetisaurus G2P: ${PHONETISAURUS_G2P}")
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(test_correctness tests/vector_matrix_test.cpp)
  target_compile_features(test_correctness PRIVATE cxx_std_20)
  target_include_directories(test_correctness PRIVATE "${CMAKE_SOURCE_DIR}/src")
  add_test(NAME correctness COMMAND test_correctness)
endif()

message(STATUS "")
message(STATUS "  ┌─────────────────────────────────────────┐")
message(STATUS "  │  TinyTTS ${PROJECT_VERSION} — ${CMAKE_BUILD_TYPE}")
message(STATUS "  │")
message(
  STATUS
    "  │  Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  │  Build dir  : ${CMAKE_BINARY_DIR}")
message(STATUS "  │  CUDA       : ${TINY_TTS_USE_CUDA}")

if(TINY_TTS_USE_CUDA)
  message(STATUS "  │  CUDA ver   : ${CMAKE_CUDA_COMPILER_VERSION}")
endif()

message(STATUS "  └─────────────────────────────────────────┘")
message(STATUS "")
