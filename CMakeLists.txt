cmake_minimum_required(VERSION 2.8)
project(MotorControl)

set (CMAKE_CXX_STANDARD 98)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")

option (USE_GUI "GUI" OFF)
option (USE_SERVER "SERVER" OFF)
option (USE_CLIENT "CLIENT" ON)

#definitions
option (USE_BIG_XRAY "Big Xray Box" OFF)
option (USE_LASER "Laser Box" OFF)
option (USE_VACUUM "Vacuum Box" OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()


if (USE_CLIENT)
    add_subdirectory(Client)
endif(USE_CLIENT)

if (USE_SERVER)
    add_subdirectory(Server)
endif(USE_SERVER)

if (USE_GUI)  
    find_package(Qt4 REQUIRED)
    add_subdirectory(Gui)
endif (USE_GUI)






