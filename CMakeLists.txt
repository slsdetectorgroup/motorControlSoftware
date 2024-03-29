cmake_minimum_required(VERSION 3.12)
project(MotorControl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})



option (USE_GUI "GUI" OFF)
option (USE_SERVER "SERVER" ON)
option (USE_CLIENT "CLIENT" ON)

#definitions
option (USE_BIG_XRAY "Big Xray Box" OFF)
option (USE_LASER "Laser Box" OFF)
option (USE_VACUUM "Vacuum Box" OFF)
option (USE_GENERIC "Generic" OFF)
option (USE_VIRTUAL "Virtual Server" OFF)
option (USE_SANITIZER "Sanitizers for debugging" OFF)


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


#Add warnings
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)


if(USE_SANITIZER)
    target_compile_options(project_warnings INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_libraries(project_warnings INTERFACE -fsanitize=address,undefined)
    # need to run this before running the server in rasp pi
    # export LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.6
endif()


if (USE_CLIENT)
    add_subdirectory(Client)
endif(USE_CLIENT)

if (USE_SERVER)
    add_subdirectory(Server)
endif(USE_SERVER)

if (USE_GUI)  
    add_subdirectory(Gui)
endif (USE_GUI)






