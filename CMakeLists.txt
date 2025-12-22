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
# Only set policy if CMake version supports it (3.30+)
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Platform-specific Boost configuration
if(WIN32)
    # On Windows, use the submodule boost (same as Linux for consistency)
    set(Boost_INCLUDE_DIRS 
        "${CMAKE_SOURCE_DIR}/external/boost"
    )
    message(STATUS "Using Boost submodule: ${Boost_INCLUDE_DIRS}")
else()
    # On Linux, prefer system Boost first, fallback to submodule
    find_package(Boost QUIET)
    if(Boost_FOUND)
        message(STATUS "Found system Boost: ${Boost_INCLUDE_DIRS}")
    else()
        # Use submodule if system Boost not found
        set(Boost_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/external/boost")
        message(STATUS "System Boost not found, using submodule: ${Boost_INCLUDE_DIRS}")
    endif()
endif()

# Define BOOST/ASIO preprocessor definitions
if(WIN32)
    add_compile_definitions(
        BOOST_ASIO_NO_DEPRECATED
        _WIN32_WINNT=0x0601  # Windows 7 or later for ASIO
    )
else()
    add_compile_definitions(
        BOOST_ASIO_NO_DEPRECATED
    )
endif()

# -----------------
# GLFW
# -----------------
if(WIN32)
    # Windows: Use precompiled library (or could use submodule too)
    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libraries/include")
    set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/libraries/lib/glfw3.lib")
else()
    # Linux: Build GLFW from submodule
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)  # Disable Wayland, use X11 only
    
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw)
    
    message(STATUS "Building GLFW from submodule")
endif()

# -----------------
# GLAD
# -----------------
add_library(glad STATIC 
    ${CMAKE_SOURCE_DIR}/libraries/include/glad/glad.c
)
target_include_directories(glad PUBLIC 
    ${CMAKE_SOURCE_DIR}/libraries/include
)

# Link dl library on Linux (required for GLAD)
if(UNIX AND NOT APPLE)
    target_link_libraries(glad PUBLIC dl)
endif()

# -----------------
# IMGUI (from submodule)
# -----------------
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

if(WIN32)
    target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/libraries/include
    )
else()
    # Add GLFW includes for imgui_impl_glfw.cpp
    target_include_directories(imgui PRIVATE
        ${CMAKE_SOURCE_DIR}/external/glfw/include
    )
endif()

target_link_libraries(imgui PUBLIC 
    glad
)

message(STATUS "Using ImGui from submodule: ${IMGUI_DIR}")

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

# CREATE THE EXECUTABLE
add_executable(app ${APP_SOURCES} ${APP_HEADERS})

# Configure includes - Boost MUST come before libraries/include
target_include_directories(app PRIVATE
    ${Boost_INCLUDE_DIRS}  # Boost first to override any stale headers
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}/src
)

if(WIN32)
    # On Windows, add libraries/include after Boost
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/libraries/include
    )
endif()

# Platform-specific linking
if(WIN32)
    target_link_libraries(app PRIVATE
        imgui
        glad
        ${GLFW_LIBRARY}
        opengl32
        ws2_32      # Windows sockets - required for ASIO
        wsock32     # Additional Windows socket library
        mswsock     # Microsoft Winsock extensions
    )
    
    # Enable parallel compilation on MSVC
    if(MSVC)
        target_compile_options(app PRIVATE /MP)
        target_compile_options(app PRIVATE /wd4996)  # Disable deprecated warnings
    endif()
else()
    # Linux: Add GLFW include directories
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/external/glfw/include
    )
    
    # Linux linking
    target_link_libraries(app PRIVATE
        imgui
        glad
        glfw               # Use the glfw target from add_subdirectory
        GL                 # OpenGL library
        pthread            # Required for threads (used by ASIO)
        X11                # X11 library (required by GLFW)
        Xrandr             # X11 Randr extension
        Xinerama           # X11 Xinerama extension
        Xcursor            # X11 Cursor extension
        Xi                 # X11 Input extension
    )
    
    # Enable warnings on GCC/Clang
    target_compile_options(app PRIVATE -Wall -Wextra)
endif()