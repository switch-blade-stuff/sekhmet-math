
include(${CMAKE_CURRENT_LIST_DIR}/detail/CMakeLists.txt)

target_sources(${SEK_MATH_PROJECT} INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/vector.hpp)