include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW64_PATH}/gcc.exe)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${GLAD_PATH}/src/glad.c)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${GLAD_PATH}/include ${GLFW_PATH}/include)
target_link_directories(${PROJECT_NAME} PUBLIC ${GLFW_PATH}/lib-mingw-w64)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw3)

