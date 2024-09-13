# Particles-Simulator

This is my personal 3D particles simulator that uses OpenGL and C++. It is a project that I started to learn OpenGL but also to learn more about particles simulation using the Verlet integration.

I also started to develop a version in Rust that has the same functionalities as the C++ version. This version might be easier to compile use and run due to the cargo package manager that provides all the dependencies needed for the project.

![](other/ParticlesSimulator.gif)

## CPP Version

### How to use

First install all the included libraries (glew, stb, glm, glfw). (I will try to make this step easier in the future)

This project was developed using g++ and CMake. To compile it, you need to have CMake and g++ installed on your machine.

then you can compile the project using the following commands :

```bash
mkdir bin
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
make
```

The executable will be in the bin folder.

MSVC version, (make sure to modify the CMakeLists.txt file to use the correct libraries path in this case, because the libraries are different from the one used in the G++ version) :
```bash
mkdir bin
mkdir build
cd build
cmake ..
cmake --build . 
cmake --build . --target clean # to clean the build
```

If you are not using a Windows machine, it is possible that you encounter some problems with the libraries or other stuff. For the moment i am not working on a compatibility version for Linux or MacOS.

## Rust Version

### How to use

There are multiple ways to run the Rust version of the project. The easiest way is to use the cargo package manager.

```bash	
cd rust
cargo run
```

This will compile and run the project. However the default version is not using the optimizations flags. Instead you should use the `bench` profile to run the optimized version.

```bash
cd rust
cargo run --profile bench
```

Due to some specific file management to make the release version deployable, you can't use the cargo package manager to run the release version. Instead you have build it first and then go to the target folder to run the executable.

```bash
cd rust
cargo build --release
cd target/release
./ParticlesSimulator.exe
```

## Features

- Particles simulation using Verlet integration.
- Collision detection and resolution between particles and the environment.
- Molecules composed of multiple particles.
- Drag and drop particles.

## Controls

- `ZQSD` : Move the camera. (also works with `WASD`)
- `E` : Move the camera up.
- `A` : Move the camera down.
- `Left Click` : Drag and drop particles.
- `Right Click` : Rotate the camera.
- `Mouse Wheel` : Zoom in and out.
- `G` : Add particles. (number of particles added can be changed in the `main.cpp|main.rs` file, default is 10)
- `T` : Attract the particles to the origin of the world.
- `P` : Pause the simulation.

## Command Line Arguments

- `--world <world_file> | -w <world_file>` : Load a world file at the start of the program.

## World and Data Files

World Files are simple JSON files that describe the starting environment of the simulation.

There are similar files existing for molecules that can describe the structure of a molecule and its properties.

You can see some basic exemples of those files in the `data` folder.

## More details

...

## Author

- [ALXS](https://github.com/ALXS-GitHub)