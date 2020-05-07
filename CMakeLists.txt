cmake_minimum_required(VERSION 2.8)
project(MotorControl)

set (CMAKE_CXX_STANDARD 98)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})



option (USE_GUI "GUI" OFF)
option (USE_SERVER "SERVER" OFF)
option (USE_CLIENT "CLIENT" ON)

#definitions
option (USE_BIG_XRAY "Big Xray Box" OFF)
option (USE_LASER "Laser Box" OFF)
option (USE_VACUUM "Vacuum Box" OFF)
option (USE_GENERIC "Generic" OFF)
option (USE_VIRTUAL "Virtual Server" OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()


set(ClangFormat_EXCLUDE_PATTERNS "build/" 
                                 "conda-recipie/" 
                                 "config/" 
                                 "docs/" 
                                 "python/"
                                 ${CMAKE_BINARY_DIR}
)
find_package(ClangFormat)


# standard compiler warnings
if (${CMAKE_MAJOR_VERSION}$ LESS "3")
    message(STATUS "CMake major version less than 3 using -Wall -Wextra instead of warning list")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
else()
    add_library(project_warnings INTERFACE)
    include(cmake/CompilerWarnings.cmake)
    set_project_warnings(project_warnings)
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






