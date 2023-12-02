cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Find the libraries
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED) 

set(SOURCES_DIR 
    src
)

file(GLOB_RECURSE SOURCES ${SOURCES_DIR}/*.cpp)

# Add the executable
add_executable(Volcano main.cpp ${SOURCES})

# Link the libraries
target_link_libraries(Volcano glfw Vulkan::Vulkan glm::glm) 

