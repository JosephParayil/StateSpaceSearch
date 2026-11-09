cmake_minimum_required(VERSION 3.16)
project(ss VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Build everything statically so there are no DLLs to ship
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ---- SFML ----
set(SFML_BUILD_AUDIO ON  CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)   # not needed
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.0        # pin an exact release
    GIT_SHALLOW    ON
    SYSTEM
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(SFML)

# ---- TGUI ----
set(TGUI_BACKEND SFML_GRAPHICS CACHE STRING "" FORCE)
set(TGUI_BUILD_GUI_BUILDER OFF CACHE BOOL "" FORCE)
set(TGUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TGUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(TGUI
    GIT_REPOSITORY https://github.com/texus/TGUI.git
    GIT_TAG        v1.12.0      # check the exact tag name on GitHub
    GIT_SHALLOW    ON
    SYSTEM
)
FetchContent_MakeAvailable(TGUI)

# ---- Your app ----
add_executable(ss main.cpp)

target_link_libraries(ss PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
    TGUI::TGUI
)

# Copy assets next to the executable (adjust folder name)
add_custom_command(TARGET ss POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/GMMP/assets $<TARGET_FILE_DIR:ss>/GMMP/assets
)

#[[
cmake_minimum_required(VERSION 3.16)
project(ss VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(TGUI_DIR "C:/Users/josep/Documents/Cpp/_PACKAGES/TGUI-1.12/build")
set(TGUI_STATIC_LIBRARIES TRUE)


find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Audio)
find_package(TGUI 1 REQUIRED)

add_executable(ss main.cpp)

target_link_libraries(ss PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio

    TGUI::TGUI
)


]]