cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(FontViewer C CXX)

file(GLOB SRC_FILES src/main.cpp)
add_executable(FontViewer ${SRC_FILES})

# Debian & Children: apt install fontconfig
# Arch & Children (Extra repo): pacman -S fontconfig
find_package(Fontconfig REQUIRED)
target_include_directories(FontViewer PUBLIC ${Fontconfig_INCLUDE_DIRS})
target_link_libraries(FontViewer PUBLIC ${Fontconfig_LIBRARIES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imbase/)
target_link_libraries(FontViewer PUBLIC imbase)

target_compile_options(FontViewer PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

