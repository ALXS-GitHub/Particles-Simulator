cmake_minimum_required(VERSION 3.8)
project(ParticlesSimulator)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# If you're using CUDA
# enable_language(CUDA)

include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4/glfw-3.4/include)
include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glew)

# Add your source files here
add_executable(ParticlesSimulator src/main.cpp src/classes/particle.cpp src/classes/simulation.cpp src/classes/renderer.cpp src/classes/camera.cpp src/dependencies/glew/glew.c)
# add_executable(ParticlesSimulator src/main.cpp src/classes/particle.cpp src/classes/simulation.cpp src/classes/renderer.cpp)

add_definitions(-DGLEW_STATIC)

include_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glm)

# link_directories(${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4/glfw-3.4/build/src/Debug)
find_library(GLFW_LIBRARY NAMES glfw3 PATHS ${PROJECT_SOURCE_DIR}/src/dependencies/glfw-3.4/glfw-3.4/build/src/Debug)

target_link_libraries(ParticlesSimulator PRIVATE ${GLFW_LIBRARY} opengl32)
# target_link_libraries(ParticlesSimulator PRIVATE glew32 glfw3 opengl32)

# If you're using any libraries, find them and link them here
# find_package(SomeLibrary REQUIRED)
# target_link_libraries(3DParticleSimulator SomeLibrary::SomeLibrary)