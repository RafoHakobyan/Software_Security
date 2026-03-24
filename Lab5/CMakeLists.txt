cmake_minimum_required(VERSION 3.10)

project(USBProtection)

set(CMAKE_CXX_STANDARD 17)

# include headers
include_directories(include)

# collect all cpp files
file(GLOB SOURCES "src/*.cpp")

# create executable
add_executable(usb_protection ${SOURCES} "include/utils.h" "src/utils.cpp" "include/usb_detector.h" "src/usb_detector.cpp" "include/cipher.h" "src/cipher.cpp" "include/file_operations.h" "src/file_operations.cpp")
