include(ProcessorCount)
ProcessorCount(NUM_THREADS)
message("Using ${NUM_THREADS} threads for building")

include(FetchContent)
FetchContent_Declare(
        libopencm3
        GIT_REPOSITORY https://github.com/libopencm3/libopencm3.git
        GIT_TAG "87a080c94ce67643216464821c752c1c406c6414")
FetchContent_MakeAvailable(libopencm3)

add_custom_target(libopencm3_build
        COMMAND make TARGETS=stm32/${MCU_FAMILY} -j ${NUM_THREADS}
        WORKING_DIRECTORY ${libopencm3_SOURCE_DIR})

add_custom_target(libopencm3_genlink
        COMMAND bash ./genlink.sh ${libopencm3_SOURCE_DIR} ${MCU_TYPE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set(OPENCM3_LIBS "${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32${MCU_FAMILY}.a")
set(OPENCM3_INCLUDES "${libopencm3_SOURCE_DIR}/include")
set(OPENCM3_LD_DIR "${libopencm3_SOURCE_DIR}/lib")
add_library(opencm3 STATIC IMPORTED)
set_target_properties(opencm3 PROPERTIES
        IMPORTED_LOCATION ${OPENCM3_LIBS}
        INTERFACE_INCLUDE_DIRECTORIES "${OPENCM3_INCLUDES}")
add_dependencies(opencm3 libopencm3_build libopencm3_genlink)