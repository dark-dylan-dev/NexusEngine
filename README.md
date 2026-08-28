# Nexus Engine

> A playground for everything I learn about game development and graphics programming.

Nexus Engine is a long-term learning project with usability as its eventual goal. 
It serves as a place for me to explore engine architecture, graphics, and tooling while keeping everything practical enough to eventually build real projects with.

The project is written in C++23 and is currently developed and tested on Windows and Linux.

## Requirements

* A C++23-compatible compiler
* CMake >= 3.30
* Ninja >= 1.11
* Git

Supported compilers:

* GCC 16
* Clang 22
* MSVC 19

### Linux packages

The following packages are required on Debian/Ubuntu-based systems using `apt`.
Adapt the package names to your distribution and package manager as needed.

```bash
apt-get update && apt-get install \
    pkg-config \
    libwayland-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxkbcommon-dev \
    wayland-protocols \
    doxygen \
    graphviz
```

## Build

Clone the repository:

```bash
git clone https://github.com/dark-dylan-dev/NexusEngine.git
cd NexusEngine
```

Configure and build the project with CMake and Ninja:

```bash
cmake -B Build -G Ninja
cmake --build Build
```

Additional options can be passed during configuration:

```bash
cmake -B Build -G Ninja \
    -DNEXUS_BUILD_TEST=ON \
    -DCMAKE_BUILD_TYPE=Release
```

You can also use parallel compilation:

```bash
cmake --build Build --parallel <NUMBER_OF_CORES>
```

## Tests

Nexus Engine uses Google Test for unit testing.

Enable tests during configuration:

```bash
cmake -B Build -G Ninja -DNEXUS_BUILD_TEST=ON
```

Then run them with:

```bash
ctest --test-dir Build/Tests --output-on-failure
```

## Documentation

API documentation is generated using Doxygen:

```bash
doxygen Doxyfile
```

## License

Nexus Engine is licensed under the MIT License.

###### Nexus Engine &copy; 2026 Dylan Hollemaert
