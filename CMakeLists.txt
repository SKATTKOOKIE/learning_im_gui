cmake_minimum_required(VERSION 3.20)
project(OpenGL_ImGui_Project)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------
# BOOST (Header-only for Beast/ASIO)
# -----------------
# Tell CMake to use the new Boost finding behavior
cmake_policy(SET CMP0167 NEW)

# Manually set Boost paths
set(BOOST_ROOT "C:/Program Files/Boost/boost_1_90_0")
set(Boost_INCLUDE_DIRS "${BOOST_ROOT}")

# Define BOOST/ASIO preprocessor definitions
add_compile_definitions(
    BOOST_ASIO_NO_DEPRECATED
    _WIN32_WINNT=0x0601  # Windows 7 or later for ASIO
)

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
# Core sources
file(GLOB CORE_SOURCES "${CMAKE_SOURCE_DIR}/src/core/*.cpp")
file(GLOB CORE_HEADERS "${CMAKE_SOURCE_DIR}/src/core/*.h")

# Component sources (includes websocket automatically)
file(GLOB_RECURSE COMPONENT_SOURCES "${CMAKE_SOURCE_DIR}/src/components/**/*.cpp")
file(GLOB_RECURSE COMPONENT_HEADERS "${CMAKE_SOURCE_DIR}/src/components/**/*.h")

# Main file
set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/src/Main.cpp")

# Combine all sources
set(APP_SOURCES 
    ${MAIN_SOURCE}
    ${CORE_SOURCES}
    ${COMPONENT_SOURCES}
)

set(APP_HEADERS
    ${CORE_HEADERS}
    ${COMPONENT_HEADERS}
)

# CREATE THE EXECUTABLE FIRST
add_executable(app ${APP_SOURCES} ${APP_HEADERS})

# THEN configure it
target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/include
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(app PRIVATE
    imgui
    glad
    ${GLFW_LIBRARY}
    opengl32
    ws2_32      # Windows sockets - required for ASIO
    wsock32     # Additional Windows socket library
    mswsock     # Microsoft Winsock extensions
)

# Optional: Enable parallel compilation on MSVC
if(MSVC)
    target_compile_options(app PRIVATE /MP)
    # Disable specific MSVC warnings if needed
    target_compile_options(app PRIVATE /wd4996)  # Disable deprecated warnings
endif()