cmake_minimum_required (VERSION 3.0.0)
include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckCSourceCompiles)
include(CMakePushCheckState)
include(CheckCCompilerFlag)

file(GLOB_RECURSE SatDump_core_CPPS  *.cpp 
                                     modules/common/aec/*.c
)

find_package(PkgConfig)

add_library(satdump_core SHARED ${SatDump_core_CPPS})
target_include_directories(satdump_core PUBLIC .)

# Threads
find_package(Threads REQUIRED)
target_link_libraries(satdump_core PUBLIC Threads::Threads)

if(MSVC)
    target_link_libraries(satdump_core PUBLIC dsp.dll)
    target_link_libraries(satdump_core PUBLIC correct.dll)
    target_link_libraries(satdump_core PUBLIC jpeg)
    target_link_libraries(satdump_core PUBLIC fmt.dll)
    target_link_libraries(satdump_core PUBLIC volk.dll)
    target_link_libraries(satdump_core PUBLIC libpng16.dll)
else()
    # LibDSP
    find_library(DSP_LIBRARY dsp REQUIRED)
    target_link_libraries(satdump_core PUBLIC ${DSP_LIBRARY})

    # LibCorrect
    find_library(CORRECT_LIBRARY correct REQUIRED)
    target_link_libraries(satdump_core PUBLIC ${CORRECT_LIBRARY})

    # LibJpeg
    find_library(JPEG_LIBRARY jpeg REQUIRED)
    target_link_libraries(satdump_core PUBLIC ${JPEG_LIBRARY})

    # FMT
    find_package(fmt REQUIRED)
    target_include_directories(satdump_core PUBLIC ${fmt_INCLUDE_DIRS})
    target_link_libraries(satdump_core PUBLIC fmt::fmt)

    # FMT
    pkg_check_modules(VOLK REQUIRED volk)
    target_include_directories(satdump_core PUBLIC ${volk_INCLUDE_DIRS})
    target_link_libraries(satdump_core PUBLIC volk)

    # LibPNG
    if(WIN32 AND NOT MINGW)
        find_package(PNG CONFIG REQUIRED)
    else()
        find_package(PNG REQUIRED)
    endif()
    target_link_libraries(satdump_core PUBLIC PNG::PNG)
endif()