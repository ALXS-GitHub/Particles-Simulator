cmake_minimum_required(VERSION 3.8)
project(ParticlesSimulator)

set(CMAKE_CXX_STANDARD 14)
SET( CMAKE_CXX_COMPILER g++) # cmake -G "MinGW Makefiles" ..

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# If you're using CUDA
# enable_language(CUDA)

# include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4/glfw-3.4/include) # msvc
include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4.bin.WIN64/glfw-3.4.bin.WIN64/include) # g++
include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glm)
include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glew)
include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/stb)
include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/json)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /O2 /Wall /W4 /openmp") # msvc
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -Wall -Wextra -fopenmp") # g++

# Add your source files here
add_executable(ParticlesSimulator 
    src/main.cpp 
    src/classes/particle.cpp 
    src/classes/particles/sphere.cpp
    src/classes/simulation.cpp 
    src/classes/renderer.cpp 
    src/classes/camera.cpp 
    src/classes/plane.cpp
    src/classes/mesh.cpp
    src/classes/container.cpp
    src/classes/containers/cubeContainer.cpp
    src/classes/containers/sphereContainer.cpp
    src/classes/grid.cpp
    src/classes/molecule.cpp
    src/utils/camera_utils.cpp
    src/utils/texture_utils.cpp
    src/utils/drag_particles.cpp
    src/utils/ray.cpp
    src/dependencies/glew/glew.c
)
# add_executable(ParticlesSimulator src/main.cpp src/classes/particle.cpp src/classes/simulation.cpp src/classes/renderer.cpp)

add_definitions(-DGLEW_STATIC)


# link_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4/glfw-3.4/build/src/Debug)
# find_library(GLFW_LIBRARY NAMES glfw3 PATHS ${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4/glfw-3.4/build/src/Debug) # msvc
find_library(GLFW_LIBRARY NAMES glfw3 PATHS ${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4.bin.WIN64/glfw-3.4.bin.WIN64/lib-mingw-w64) # g++

target_link_libraries(ParticlesSimulator PRIVATE ${GLFW_LIBRARY} opengl32)
# target_link_libraries(ParticlesSimulator PRIVATE glew32 glfw3 opengl32)

# If you're using any libraries, find them and link them here
# find_package(SomeLibrary REQUIRED)
# target_link_libraries(3DParticleSimulator SomeLibrary::SomeLibrary)