cmake_minimum_required(VERSION 3.20)
project(sekhmet-math)

# Library options
option(SEK_FORCE_LEFT_HANDED "Forces the use of left-handed coordinate system" OFF)
if (${SEK_FORCE_LEFT_HANDED})
    add_compile_definitions(SEK_FORCE_LEFT_HANDED)
endif ()

option(SEK_MATH_BUILD_OBJECT "Toggles build of object library target" ON)
option(SEK_MATH_BUILD_SHARED "Toggles build of shared library target" ON)
option(SEK_MATH_BUILD_STATIC "Toggles build of static library target" ON)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE STRING "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE STRING "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE STRING "")

# Include library source subdirectory
set(SEK_MATH_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/math)
include(${SEK_MATH_SOURCE_DIR}/CMakeLists.txt)

# Add project targets & include directories
if (MSVC)
    set(SEK_MATH_COMPILE_OPTIONS /W3 /WX)
else ()
    set(SEK_MATH_COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -Wno-ignored-attributes)
endif ()

if (SEK_MATH_BUILD_OBJECT)
    add_library(${PROJECT_NAME}-object OBJECT)
    target_sources(${PROJECT_NAME}-object PUBLIC ${SEK_MATH_PUBLIC_SOURCES})
    target_sources(${PROJECT_NAME}-object PRIVATE ${SEK_MATH_PRIVATE_SOURCES})
    target_sources(${PROJECT_NAME}-object INTERFACE ${SEK_MATH_INTERFACE_SOURCES})
    target_include_directories(${PROJECT_NAME}-object PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    set_target_properties(${PROJECT_NAME}-object PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

    # Set C++ version
    target_compile_features(${PROJECT_NAME}-object PUBLIC cxx_std_20)

    # Set symbol visibility
    set_target_properties(${PROJECT_NAME}-object PROPERTIES C_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME}-object PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME}-object PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

    # Set shared library definitions
    set_target_properties(${PROJECT_NAME}-object PROPERTIES DEFINE_SYMBOL "SEK_MATH_EXPORT")
    target_compile_definitions(${PROJECT_NAME}-object PUBLIC SEK_MATH_LIB_SHARED)

    # Enable max error reporting
    target_compile_options(${PROJECT_NAME}-object PRIVATE ${SEK_MATH_COMPILE_OPTIONS})
endif ()
if (SEK_MATH_BUILD_SHARED)

    add_library(${PROJECT_NAME}-shared SHARED)
    target_sources(${PROJECT_NAME}-shared PUBLIC ${SEK_MATH_PUBLIC_SOURCES})
    target_sources(${PROJECT_NAME}-shared PRIVATE ${SEK_MATH_PRIVATE_SOURCES})
    target_sources(${PROJECT_NAME}-shared INTERFACE ${SEK_MATH_INTERFACE_SOURCES})
    target_include_directories(${PROJECT_NAME}-shared PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    set_target_properties(${PROJECT_NAME}-shared PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

    # Set C++ version
    target_compile_features(${PROJECT_NAME}-shared PUBLIC cxx_std_20)

    # Set symbol visibility
    set_target_properties(${PROJECT_NAME}-shared PROPERTIES C_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME}-shared PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME}-shared PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

    # Set shared library definitions
    set_target_properties(${PROJECT_NAME}-shared PROPERTIES DEFINE_SYMBOL "SEK_MATH_EXPORT")
    target_compile_definitions(${PROJECT_NAME}-shared PUBLIC SEK_MATH_LIB_SHARED)

    # Enable max error reporting
    target_compile_options(${PROJECT_NAME}-shared PRIVATE ${SEK_MATH_COMPILE_OPTIONS})
endif ()
if (SEK_MATH_BUILD_STATIC)
    add_library(${PROJECT_NAME}-static STATIC)
    target_sources(${PROJECT_NAME}-static PUBLIC ${SEK_MATH_PUBLIC_SOURCES})
    target_sources(${PROJECT_NAME}-static PRIVATE ${SEK_MATH_PRIVATE_SOURCES})
    target_sources(${PROJECT_NAME}-static INTERFACE ${SEK_MATH_INTERFACE_SOURCES})
    target_include_directories(${PROJECT_NAME}-static PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

    # Set C++ version
    target_compile_features(${PROJECT_NAME}-static PUBLIC cxx_std_20)

    # Set symbol visibility
    set_target_properties(${PROJECT_NAME}-static PROPERTIES C_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME}-static PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME}-static PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

    # Set shared library definitions
    set_target_properties(${PROJECT_NAME}-static PROPERTIES DEFINE_SYMBOL "SEK_MATH_EXPORT")
    target_compile_definitions(${PROJECT_NAME}-static PUBLIC SEK_MATH_LIB_STATIC)

    # Enable max error reporting
    target_compile_options(${PROJECT_NAME}-static PRIVATE ${SEK_MATH_COMPILE_OPTIONS})
endif ()

# Add DPM subproject
set(DPM_HANDLE_ERRORS ON CACHE BOOL "Enable error handling for vectorized math functions")
set(DPM_INLINE_EXTENSIONS ON CACHE BOOL "Bring DPM math extensions into top-level namespace")
set(DPM_BUILD_SHARED OFF CACHE BOOL "Disable DPM shared library target")
set(DPM_BUILD_STATIC OFF CACHE BOOL "Disable DPM static library target")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/dpm)

# Link DPM as public dependency
function(sek_target_link_objects TARGET MODE OBJ_LIBRARY)
    target_link_libraries(${TARGET} ${MODE} ${OBJ_LIBRARY} $<TARGET_OBJECTS:${OBJ_LIBRARY}>)
endfunction()
if (SEK_MATH_BUILD_OBJECT)
    sek_target_link_objects(${PROJECT_NAME}-object PUBLIC dpm-object)
endif ()
if (SEK_MATH_BUILD_SHARED)
    sek_target_link_objects(${PROJECT_NAME}-shared PUBLIC dpm-object)
endif ()
if (SEK_MATH_BUILD_STATIC)
    sek_target_link_objects(${PROJECT_NAME}-static PUBLIC dpm-object)
endif ()

# Add unit tests
option(SEK_MATH_TESTS "Enable unit tests" OFF)
if (${SEK_MATH_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
endif ()