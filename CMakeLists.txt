cmake_minimum_required(VERSION 3.20)
project(sekhmet-math)

# Library options
option(SEK_FORCE_LEFT_HANDED "Forces the use of left-handed coordinate system" OFF)
if (${SEK_FORCE_LEFT_HANDED})
    add_compile_definitions(SEK_FORCE_LEFT_HANDED)
endif ()

option(BUILD_SHARED_LIBS "Build sekhmet-math as a shared library" OFF)

# Add project targets
set(SEK_MATH_PROJECT ${PROJECT_NAME})
add_library(${SEK_MATH_PROJECT})

# Set features & compile options
target_compile_features(${SEK_MATH_PROJECT} PUBLIC cxx_std_20)

# Enable max error reporting
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -Wno-ignored-attributes)
endif ()

# Include library source subdirectory
set(SEK_MATH_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/math)
include(${SEK_MATH_SOURCE_DIR}/CMakeLists.txt)

# Set symbol visibility
set_target_properties(${SEK_MATH_PROJECT} PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(${SEK_MATH_PROJECT} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${SEK_MATH_PROJECT} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

# Set shared library definitions
set_target_properties(${SEK_MATH_PROJECT} PROPERTIES DEFINE_SYMBOL "SEK_MATH_EXPORT")
target_compile_definitions(${SEK_MATH_PROJECT} PUBLIC $<IF:$<STREQUAL:$<TARGET_PROPERTY:${SEK_MATH_PROJECT},TYPE>,SHARED_LIBRARY>,SEK_MATH_LIB_SHARED,SEK_MATH_LIB_STATIC>)

# Add DPM subproject & link it as public dependency
set(DPM_HANDLE_ERRORS ON CACHE BOOL "Enable error handling for vectorized math functions")
set(DPM_INLINE_EXTENSIONS ON CACHE BOOL "Bring DPM math extensions into top-level namespace")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/dpm)
target_link_libraries(${SEK_MATH_PROJECT} PUBLIC dpm)
target_include_directories(${SEK_MATH_PROJECT} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/dpm)

# Add unit tests
option(SEK_MATH_TESTS "Enable unit tests" OFF)
if (${SEK_MATH_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
endif ()