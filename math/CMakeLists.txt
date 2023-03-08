
include(${CMAKE_CURRENT_LIST_DIR}/detail/CMakeLists.txt)

list(APPEND SEK_MATH_INTERFACE_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/scalar.hpp
        ${CMAKE_CURRENT_LIST_DIR}/vector.hpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix.hpp
        ${CMAKE_CURRENT_LIST_DIR}/quaternion.hpp
        ${CMAKE_CURRENT_LIST_DIR}/bounds.hpp
        ${CMAKE_CURRENT_LIST_DIR}/random.hpp
        ${CMAKE_CURRENT_LIST_DIR}/math.hpp)