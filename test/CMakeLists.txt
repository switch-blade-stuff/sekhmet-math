cmake_minimum_required(VERSION 3.20)
project(sekhmet-math-tests)
enable_testing()

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
if (SEK_MATH_BUILD_STATIC)
    target_link_libraries(${PROJECT_NAME} PRIVATE sekhmet-math-static)
elseif (SEK_MATH_BUILD_SHARED)
    target_link_libraries(${PROJECT_NAME} PRIVATE sekhmet-math-shared)
endif ()

# Enable max error reporting
target_compile_options(${PROJECT_NAME} PUBLIC ${SEK_MATH_COMPILE_OPTIONS})