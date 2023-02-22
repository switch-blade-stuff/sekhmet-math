
include(${CMAKE_CURRENT_LIST_DIR}/vector/CMakeLists.txt)

target_sources(${SEK_MATH_PROJECT} INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/fwd.hpp)