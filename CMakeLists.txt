# cmake configuration
cmake_minimum_required(VERSION 3.9...3.19)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
            "In-tree builds are not supported; please perform an out-of-tree build:\n"
            "    rm -rf CMakeCache.txt CMakeFiles/\n"
            "    mkdir build && cd build && cmake ..")
endif()

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# for /MT on MSVC
set(CMAKE_USER_MAKE_RULES_OVERRIDE
   "${CMAKE_SOURCE_DIR}/cmake/c_flag_overrides.cmake")
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   "${CMAKE_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake")

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if (APPLE)
    # Docs say this must be set before the first project() call
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "macOS minimum supported version")
endif()

# project

project(jartsy
    VERSION 0.0001
    LANGUAGES C CXX ASM)

set(ENABLE_CLI        ON CACHE BOOL
    "Whether the command line interface is enabled")
set(ENABLE_SANITIZERS OFF CACHE BOOL
    "Whether to enable Clang's AddressSanitizer and UndefinedBehaviorSanitizer")
set(ENABLE_OPENMP     OFF CACHE BOOL
    "Whether geometric operations will be parallelized using OpenMP")
set(ENABLE_LTO        OFF CACHE BOOL
    "Whether interprocedural (global) optimizations are enabled")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/bin)
if("${CMAKE_GENERATOR}" STREQUAL "Xcode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
        # GCC 4.8/4.9 ship with broken but present <regex>. meh.
        message(FATAL_ERROR "GCC 5.0+ is required")
    endif()
endif()

# common compiler flags
include(CheckCXXCompilerFlag)

if (NOT APPLE)
    set(FILE_PREFIX_MAP "-ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.")
    check_cxx_compiler_flag("${FILE_PREFIX_MAP}" HAS_FILE_PREFIX_MAP)
    if(HAS_FILE_PREFIX_MAP)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FILE_PREFIX_MAP}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FILE_PREFIX_MAP}")
    endif()
endif()

if(MINGW)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# Ensure that all platforms use 64-bit IEEE floating point operations for consistency;
# this is most important for the testsuite, which compares savefiles directly
# and depends on consistent rounding of intermediate results.
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "i686" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "X86")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(FLOAT_FLAGS "-mfpmath=sse -msse2")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(FLOAT_FLAGS "-msse2")
    endif()

    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${FLOAT_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLOAT_FLAGS}")
endif()

if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(ENABLE_OPENMP)
    find_package( OpenMP REQUIRED )
    if(OPENMP_FOUND)
        add_library(jartsy_openmp INTERFACE)
        target_compile_options(jartsy_openmp INTERFACE ${OpenMP_CXX_FLAGS})
        target_link_libraries(jartsy_openmp INTERFACE
            ${OpenMP_CXX_LIBRARIES})
        target_include_directories(jartsy_openmp INTERFACE SYSTEM
            ${OpenMP_CXX_INCLUDE_DIRS})
        message(STATUS "found OpenMP, compiling with flags: " ${OpenMP_CXX_FLAGS} )
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_EXE_LINKER_FLAGS}")
endif()

if(ENABLE_SANITIZERS)
    if(NOT SANITIZERS)
        set(SANITIZERS "address;undefined")
    endif()

    if("thread" IN_LIST SANITIZERS)
        list(REMOVE_ITEM SANITIZERS "thread")
        list(APPEND SANITIZE_OPTIONS thread)
    endif()
    if("address" IN_LIST SANITIZERS)
        list(REMOVE_ITEM SANITIZERS "address")
        list(APPEND SANITIZE_OPTIONS address)
    endif()
    if("undefined" IN_LIST SANITIZERS)
        list(REMOVE_ITEM SANITIZERS "undefined")
        list(APPEND SANITIZE_OPTIONS alignment bounds)
        list(APPEND SANITIZE_OPTIONS shift signed-integer-overflow integer-divide-by-zero)
        list(APPEND SANITIZE_OPTIONS null bool enum)
        list(APPEND SANITIZE_OPTIONS return)
    endif()
    if(SANITIZERS)
        message(FATAL_ERROR "Unknown sanitizer(s) ${SANITIZERS}")
    else()
        message(STATUS "Using sanitizer options ${SANITIZE_OPTIONS}")
    endif()

    string(REPLACE ";" "," SANITIZE_OPTIONS "${SANITIZE_OPTIONS}")

    if (NOT APPLE)
        set(SANITIZE_FLAGS "-O1 -fsanitize=${SANITIZE_OPTIONS} -fno-sanitize-recover=address,undefined")
    else()
        set(SANITIZE_FLAGS "-O1 -fsanitize=${SANITIZE_OPTIONS}")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(SANITIZE_FLAGS "${SANITIZE_FLAGS} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(SANITIZE_FLAGS "${SANITIZE_FLAGS} -fuse-ld=gold")
    else()
        message(FATAL_ERROR "Sanitizers are only available when using GCC or Clang")
    endif()

    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${SANITIZE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZE_FLAGS}")
endif()

# common dependencies

if(APPLE)
    set(CMAKE_FIND_FRAMEWORK LAST)
endif()

# jartsy-only compiler flags

if(WIN32)
    add_definitions(
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -DWINVER=0x0501
        -D_WIN32_WINNT=0x0501
        -D_WIN32_IE=_WIN32_WINNT
        -DISOLATION_AWARE_ENABLED
        -DWIN32
        -DWIN32_LEAN_AND_MEAN
        -DUNICODE
        -D_UNICODE
        -DNOMINMAX
        -D_USE_MATH_DEFINES)
endif()

if(MSVC)
    # Many versions of MSVC do not have the (C99) inline keyword, instead
    # they have their own __inline; this breaks `static inline` functions.
    # We do not want to care and so we fix this with a definition.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Dinline=__inline")
    # Same for the (C99) __func__ special variable; we use it only in C++ code.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D__func__=__FUNCTION__")

    # We rely on these /we flags. They correspond to the GNU-style flags below as
    # follows: /w4062=-Wswitch
    set(WARNING_FLAGS   "${WARNING_FLAGS} /we4062")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WARNING_FLAGS   "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(WARNING_FLAGS "${WARNING_FLAGS} -Wfloat-conversion")
    endif()
    # We rely on these -Werror flags.
    set(WARNING_FLAGS   "${WARNING_FLAGS} -Werror=switch")
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")

if(WIN32)
    set(CMAKE_RC_FLAGS  "${CMAKE_RC_FLAGS} -l0")
endif()

# application components

#add_subdirectory(res)
add_subdirectory(src)
