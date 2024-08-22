cmake_minimum_required(VERSION 3.10) # Mise à jour pour une version plus récente, évite la dépréciation

message(STATUS "Compiling client software")
message(STATUS "CMake version: ${CMAKE_VERSION}")

project(jetsonSoftware-client)


# Set C and CXX compiler
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check OpenCV installation
message (STATUS "Checking for OpenCV")
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message (STATUS "OpenCV found")
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message (FATAL_ERROR "OpenCV not found")
endif()

# Check pthread
message (STATUS "Checking for pthread")
find_package(Threads REQUIRED)
if (Threads_FOUND)
    message (STATUS "pthread found")
else()
    message (FATAL_ERROR "pthread not found")
endif()

# Check GStreamer installation
message (STATUS "Checking for GStreamer")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER-APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GSTREAMER-VIDEO REQUIRED gstreamer-video-1.0)
pkg_check_modules(GSTREAMER-BASE REQUIRED gstreamer-base-1.0)

if (GSTREAMER_FOUND AND GSTREAMER-APP_FOUND AND GSTREAMER-VIDEO_FOUND AND GSTREAMER-BASE_FOUND)
    message (STATUS "GStreamer found")
    include_directories(${GSTREAMER_INCLUDE_DIRS})
else()
    message (FATAL_ERROR "GStreamer not found")
endif()

#checking openssl installation
message (STATUS "Checking for OpenSSL")
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    message (STATUS "OpenSSL found")
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message (FATAL_ERROR "OpenSSL not found")
endif()


# Set compiler flags
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-local-typedefs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field -O3")


# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Set include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pipeline)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xAssert)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/XsSignature)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})



# Set source files
set(SOURCE_FILES
        pipeline/xPipeline.cpp
        pipeline/xPipeline.h
        xAssert/xAssert.h
        xAssert/xAssert.c
        XsSignature/xSignature.h
        XsSignature/xSignature.c
)

#create so library
add_library(jetsonSoftware-client SHARED ${SOURCE_FILES})

# Link libraries
target_link_libraries(jetsonSoftware-client ${OpenCV_LIBS})
target_link_libraries(jetsonSoftware-client ${GSTREAMER_LIBRARIES})
target_link_libraries(jetsonSoftware-client ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(jetsonSoftware-client ${OPENSSL_LIBRARIES})

# Install
install(TARGETS jetsonSoftware-client DESTINATION bin)
