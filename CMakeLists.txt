include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW64}gcc.exe)
set(CMAKE_CXX_COMPILER ${MINGW64}g++.exe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyProject VERSION 1.0)

include_directories(${MY_UTILS_PATH})

add_executable(${PROJECT_NAME}
  ${SRC_DIR}main.cpp
  ${GLAD}/src/glad.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${GLFW}/include
  ${GLAD}/include
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GLFW}/lib-mingw-w64
)

target_link_libraries(${PROJECT_NAME} glfw3 opengl32)

