cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Nanokey)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

project(${CMAKE_PROJECT_NAME})

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "firmware")

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    firmware.c
    hal.c
    keyboardhid.c

    #ST USB MIDDLEWARE SOURCE FILES
    ST_USB_MW/usbd_conf.c
    ST_USB_MW/usbd_core.c
    ST_USB_MW/usbd_ctlreq.c
    ST_USB_MW/usbd_desc.c
    ST_USB_MW/usbd_ioreq.c

    #BLUETOOTH MODULE
    bt.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
   Core/Inc
   ST_USB_MW
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)