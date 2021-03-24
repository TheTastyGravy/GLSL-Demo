# GLSL Project
Custom graphics implementation in C++ and GLSL, with AIE bootstrap

## Building
Bootstrap is rather error prone, so building can be problematic. When building bootstrap, one configuration will cause a linker error, so try both if one doesnt work. Beyond that, errors are more unique and have to be solved on a case-by-case basis.

After building bootstrap, the GraphicsProject project can be built, containing the graphics demo.

## Usage
The project involves rendering models with textures with custom shaders writen in GLSL, directional and point lighting, and particle emitters. 
Models and lighing are handeled in the Scene class, owned by the GraphicsProjectApp class, which exposes values of lighting, object transforms, materials, etc, with ImGui for editing at runtime.
The ParticleEmitter class is owned by PraphicsProjectApp, and creates particles instanced on the GPU using a custom shader.

All custom shaders can be found in bin/shaders/.
