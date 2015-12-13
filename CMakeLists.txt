cmake_minimum_required (VERSION 2.8.11)
project (BattleShip)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat \
                        -Wno-c++98-compat-pedantic -Wno-padded")
  set (CMAKE_CXX_FLAGS_DEBUG "-O1 -g -fsanitize=address -fsanitize=undefined")
  set (CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "-Wpedantic -Wall -Wextra \
                        -Wno-missing-field-initializers")
  set (CMAKE_CXX_FLAGS_DEBUG "-Og -g")
  set (CMAKE_CXX_FLAGS_RELEASE "-O2 -flto")
endif ()

find_package(Qt5Widgets)
set(CMAKE_AUTOMOC ON)

add_subdirectory (src)
