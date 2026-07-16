# Nexus Engine

## Summary
- [Brief](#brief)
    - [Toolchain](#toolchain)
    - [Availability](#availability) 
- [Modules](#modules)
    - [Filesystem](#filesystem)
    - [Graphics](#graphics)
    - [ECS](#ecs)
    - [Physics](#physics)
    - [Audio](#audio)
    - [UI](#ui)
    - [Scripting](#scripting)
    - [Window management](#window-management)

# Brief

## Toolchain
- _Support might be available in previous versions, although not guaranteed_
- C++26
- Build toolchain
    - CMake >= 4.3
    - Ninja >= 1.13
- Compilers
    - GCC >= 16
    - Clang >= 22
    - MSVC >= 19
- IDE / Code editor
    - Works with anything, I use CLion
- Documentation
    - Doxygen
- Code analysis / format
    - Clang tidy
    - Clang format

## Availability
- Builds on both Windows, Linux and Android
- French and English language support

# Modules

## Filesystem
- File watchers
- 3D model support : .obj, .fbx, .glb, .gltf
- Control over where the projects are stored
- Directory viewer inside the Editor

## Graphics
- RHI
- Render graph
- Physically Based Rendering
- GPU driven rendering
  - Frustum culling
  - Occlusion culling
  - SSAO
- Path tracing
- Ray tracing
- Texture compression
- Mipmaps
- TAA, FXAA
- Cinematic pass
- Bloom
- Blinn, Phong, Blinn-phong
- SDFs
- Upscaling methods : API-agnostic and both NVIDIA DLSS and AMD FSR

## ECS
- Templated components
- Exponentially growing allocations
- Sparse sets
- Built-in serialization methods
    - Both to binary and JSON files

## Physics
- Jolt

## Audio
- Miniaudio

## UI
- ImGui
- Qt later maybe

## Scripting
- C# 
- Lua

## Window management
- Support for both SDL and GLFW


###### _Nexus Engine v1.0 &copy; 2026 Dylan Hollemaert - Licensed under the MIT License_