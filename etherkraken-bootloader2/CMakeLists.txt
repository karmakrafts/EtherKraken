cmake_minimum_required(VERSION 3.18)
project(bootloader2 C ASM)

set(CMAKE_C_STANDARD 23)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(MCU_FAMILY "f0")
set(MCU_TYPE "stm32f071cbt6")
include(opencm3)

file(GLOB_RECURSE BL2_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE BL2_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
add_executable(bootloader2
        ${BL2_SOURCE_FILES}
        ${BL2_HEADER_FILES})
target_compile_options(bootloader2 PRIVATE
        -ffunction-sections
        -fdata-sections)
target_link_options(bootloader2 PRIVATE
        -nostdlib
        -Wl,--gc-sections
        -Wl,-T${CMAKE_SOURCE_DIR}/linker.ld)
target_link_libraries(bootloader2 PRIVATE opencm3)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_options(bootloader2 PRIVATE -g -O0)
else()
    target_compile_options(bootloader2 PRIVATE -O3)
endif()

add_custom_command(TARGET bootloader2 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary bootloader2 bootloader2.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex bootloader2 bootloader2.hex)