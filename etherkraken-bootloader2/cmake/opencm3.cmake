include_guard(GLOBAL)

include(ProcessorCount)
ProcessorCount(NUM_THREADS)
message("Using ${NUM_THREADS} threads for building")

find_program(ARM_GCC_EXECUTABLE arm-none-eabi-gcc)
if ("${ARM_GCC_EXECUTABLE}" STREQUAL "ARM_GCC_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "libopencm3 CMake support requires arm-none-eabi-gcc!")
endif ()

find_program(PYTHON_EXECUTABLE /usr/bin/python3 /usr/bin/python python3 python)
if ("${PYTHON_EXE}" STREQUAL "PYTHON_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "libopencm3 CMake support requires Python!")
endif ()

include(FetchContent)

FetchContent_Declare(
        libopencm3
        GIT_REPOSITORY https://github.com/libopencm3/libopencm3.git
        GIT_TAG "87a080c94ce67643216464821c752c1c406c6414")
FetchContent_MakeAvailable(libopencm3)

function(_genlink DEVICE PROPERTY OUTPUT)
    execute_process(COMMAND ${PYTHON_EXECUTABLE}
            ${libopencm3_SOURCE_DIR}/scripts/genlink.py
            ${libopencm3_SOURCE_DIR}/ld/devices.data
            ${DEVICE} ${PROPERTY}
            OUTPUT_VARIABLE OUT_DATA
            RESULT_VARIABLE SUCCESS)
    if ("${SUCCESS}" EQUAL "0")
        message(DEBUG ">> ${OUT_DATA}")
        set("${OUTPUT}" "${OUT_DATA}" PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Unable to obtain ${PROPERTY} for device ${DEVICE}!")
    endif ()
endfunction()

_genlink(${MCU_TYPE} DEFS _MCU_DEFS)

add_custom_target(libopencm3_build
        COMMAND ${CMAKE_MAKE_PROGRAM} TARGETS=${MCU_FAMILY} -j ${NUM_THREADS}
        WORKING_DIRECTORY ${libopencm3_SOURCE_DIR})

set(_LINKER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/linker.ld")
string(REPLACE " " ";" _PREPRO_MCU_DEFS ${_MCU_DEFS})
add_custom_target(libopencm3_genlink COMMAND ${ARM_GCC_EXECUTABLE}
        ${_PREPRO_MCU_DEFS}
        -P -E ${libopencm3_SOURCE_DIR}/ld/linker.ld.S
        -o ${_LINKER_SCRIPT})

separate_arguments(HARDWARE_DEFS NATIVE_COMMAND "${_MCU_DEFS}")
set(CLEAN_HARDWARE_DEFS "")
foreach (definition IN LISTS HARDWARE_DEFS)
    string(SUBSTRING "${definition}" 2 -1 clean_definition) # Strip -D prefix
    set(CLEAN_HARDWARE_DEFS "${CLEAN_HARDWARE_DEFS} ${clean_definition}")
endforeach ()
separate_arguments(HARDWARE_DEFS NATIVE_COMMAND "${CLEAN_HARDWARE_DEFS}")
message(STATUS "Using hardware definitions: ${HARDWARE_DEFS}")

string(REPLACE "/" "" clean_mcu_family "${MCU_FAMILY}")
set(OPENCM3_LIBS "${libopencm3_SOURCE_DIR}/lib/libopencm3_${clean_mcu_family}.a")
set(OPENCM3_INCLUDES "${libopencm3_SOURCE_DIR}/include")
set(OPENCM3_LD_DIR "${libopencm3_SOURCE_DIR}/lib")

# Find location of libgcc.a for builtin intrinsics
execute_process(COMMAND ${ARM_GCC_EXECUTABLE} -print-libgcc-file-name
        OUTPUT_VARIABLE _LIBGCC_FILE)
string(STRIP "${_LIBGCC_FILE}" _LIBGCC_FILE)
get_filename_component(_LIBGCC_DIR "${_LIBGCC_FILE}" DIRECTORY)
message(STATUS "Linking against libgcc from ${_LIBGCC_FILE}")

# Find location of libc_nano.a for MORE builtin intrinsics
execute_process(COMMAND ${ARM_GCC_EXECUTABLE} -print-file-name=libc_nano.a
        OUTPUT_VARIABLE _LIBC_FILE)
string(STRIP "${_LIBC_FILE}" _LIBC_FILE)
cmake_path(NORMAL_PATH _LIBC_FILE
        OUTPUT_VARIABLE _LIBC_FILE)
get_filename_component(_LIBC_DIR "${_LIBC_FILE}" DIRECTORY)
message(STATUS "Linking against libc from ${_LIBC_FILE}")

add_library(opencm3 STATIC IMPORTED)
target_compile_definitions(opencm3 INTERFACE ${HARDWARE_DEFS})
set_target_properties(opencm3 PROPERTIES
        IMPORTED_LOCATION ${OPENCM3_LIBS}
        INTERFACE_INCLUDE_DIRECTORIES "${OPENCM3_INCLUDES}")
set_property(TARGET opencm3
        PROPERTY INTERFACE_LINK_OPTIONS
        -L${_LIBGCC_DIR}
        -L${_LIBC_DIR}
        -l:libgcc.a
        -l:libc_nano.a
        -Wl,-T${_LINKER_SCRIPT}
        -nostdlib
        -Wl,--gc-sections)
set_property(TARGET opencm3
        PROPERTY INTERFACE_COMPILE_OPTIONS
        -ffunction-sections
        -fdata-sections)
add_dependencies(opencm3 libopencm3_build libopencm3_genlink)