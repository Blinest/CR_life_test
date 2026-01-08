cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME CR_life_test)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}



)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources her
        App/Tasks/MotorCtrlTask.c
        App/Tasks/MotorDataTask.c
        App/Tasks/PressDataTask.c
        App/Tasks/PressSignTask.c
        App/Common/utils.c
        App/Net/onenet.c
        App/Net/onenet.h
        App/Net/cJSON.c
        App/Net/cJSON.h
        App/Net/MqttKit.c
        App/Net/MqttKit.h
        App/Net/Common.h
        Drivers/BSP/Motor/Emm_V5.h
        Drivers/BSP/Motor/Emm_V5.c
        Drivers/BSP/Motor/motor.c
        Drivers/BSP/Motor/motor.h
        Drivers/BSP/PressSensor/pressSensor.c
        Drivers/BSP/PressSensor/pressSensor.h
        Drivers/fifo.c
        Drivers/fifo.h
        Core/Src/Command.c
        Core/Inc/Command.h
        App/Net/ESP8266.c
        App/Net/ESP8266.h
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        App
        Drivers
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
