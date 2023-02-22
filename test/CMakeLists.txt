cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
enable_testing()

project(sekhmet-math-tests)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE sekhmet-math)

# Enable max error reporting
if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /W3 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -Wno-logical-not-parentheses)
endif ()