# Nexus Engine: TODO

* [Availability](#availability)
* [Modules](#modules)
  * [Filesystem](#filesystem)
  * [Graphics](#graphics)
  * [ECS](#ecs)
  * [Physics](#physics)
  * [Audio](#audio)
  * [UI](#ui)
  * [Scripting](#scripting)
  * [Window management](#window-management)

# Availability

* Builds on both Windows and Linux
* French and English language support

# Modules

## Math

* Vectors
* Matrices
* Quaternions
* Transforms
* Geometric primitives

## Filesystem

* File watchers
* 3D model support : .obj, .fbx, .glb, .gltf
* Control over where the projects are stored
* Directory viewer inside the Editor

## Graphics

* RHI
* Render graph
* Physically Based Rendering
* GPU driven rendering
  * Frustum culling
  * Occlusion culling
  * SSAO
* Path tracing
* Ray tracing
* Texture compression
* Mipmaps
* TAA, FXAA
* Cinematic pass
* Bloom
* Blinn, Phong, Blinn-phong
* SDFs
* Upscaling methods : API-agnostic and both NVIDIA DLSS and AMD FSR

## ECS

* Built-in serialization methods
  * Both to binary and JSON files

## Physics

* Jolt

## Audio

* Miniaudio

## UI

* ImGui

## Scripting

* C#
* Lua

## Window management

* GLFW
* SDL
