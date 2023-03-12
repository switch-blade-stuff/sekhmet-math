cmake_minimum_required(VERSION 3.20)
project(sekhmet-math)

# Library options
option(SEK_FORCE_LEFT_HANDED "Forces the use of left-handed coordinate system" OFF)
if (${SEK_FORCE_LEFT_HANDED})
    add_compile_definitions(SEK_FORCE_LEFT_HANDED)
endif ()

option(SEK_MATH_BUILD_SHARED "Toggles build of shared library target" ON)
option(SEK_MATH_BUILD_STATIC "Toggles build of static library target" ON)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE STRING "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE STRING "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE STRING "")

# Include library source subdirectory
set(SEK_MATH_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/math)
include(${SEK_MATH_SOURCE_DIR}/CMakeLists.txt)

# Add DPM subproject
set(DPM_BUILD_SHARED ${SEK_MATH_BUILD_SHARED} CACHE BOOL "")
set(DPM_BUILD_STATIC ${SEK_MATH_BUILD_STATIC} CACHE BOOL "")
set(DPM_HANDLE_ERRORS ON CACHE BOOL "Enable error handling for vectorized math functions")
set(DPM_INLINE_EXTENSIONS ON CACHE BOOL "Bring DPM math extensions into top-level namespace")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/dpm)

# Add shared & static library targets
function(sek_configure_target NAME TYPE)
    add_library(${NAME} ${TYPE})
    target_sources(${NAME} PUBLIC ${SEK_MATH_PUBLIC_SOURCES})
    target_sources(${NAME} PRIVATE ${SEK_MATH_PRIVATE_SOURCES})
    target_sources(${NAME} INTERFACE ${SEK_MATH_INTERFACE_SOURCES})
    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

    # Set C++ version
    target_compile_features(${NAME} PUBLIC cxx_std_20)

    # Set symbol visibility
    set_target_properties(${NAME} PROPERTIES C_VISIBILITY_PRESET hidden)
    set_target_properties(${NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${NAME} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

    # Set shared library definitions
    set_target_properties(${NAME} PROPERTIES DEFINE_SYMBOL "SEK_MATH_EXPORT")

    # Enable max error reporting
    if (MSVC)
        target_compile_options(${NAME} PRIVATE /W3 /WX)
    else ()
        target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -Wno-ignored-attributes)
    endif ()
endfunction()
if (SEK_MATH_BUILD_SHARED)
    sek_configure_target(${PROJECT_NAME}-shared SHARED)
    target_link_libraries(${PROJECT_NAME}-shared PUBLIC dpm-shared)
endif ()
if (SEK_MATH_BUILD_STATIC)
    sek_configure_target(${PROJECT_NAME}-static STATIC)
    target_link_libraries(${PROJECT_NAME}-static PUBLIC dpm-static)
    set_target_properties(${PROJECT_NAME}-static PROPERTIES PREFIX "lib")
    target_compile_definitions(${PROJECT_NAME}-static PUBLIC SEK_MATH_LIB_STATIC)
endif ()

# Add unit tests
option(SEK_MATH_TESTS "Enable unit tests" OFF)
if (${SEK_MATH_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
endif ()