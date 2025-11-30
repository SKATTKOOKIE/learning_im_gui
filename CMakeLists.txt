cmake_minimum_required(VERSION 3.20)
project(OpenGL_ImGui_Project)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------
# GLFW (precompiled)
# -----------------
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libraries/include")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/libraries/lib/glfw3.lib")

# -----------------
# GLAD
# -----------------
add_library(glad STATIC 
    ${CMAKE_SOURCE_DIR}/libraries/include/glad/glad.c
)
target_include_directories(glad PUBLIC 
    ${CMAKE_SOURCE_DIR}/libraries/include
)

# -----------------
# IMGUI
# -----------------
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/libraries/include
)

target_link_libraries(imgui PUBLIC 
    glad
)

# -----------------
# MAIN APPLICATION
# -----------------
add_executable(app Main.cpp)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/include
    ${CMAKE_SOURCE_DIR}/imgui
)

target_link_libraries(app PRIVATE
    imgui
    glad
    ${GLFW_LIBRARY}
    opengl32
)