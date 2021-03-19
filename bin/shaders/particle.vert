// a simple shader
#version 410

layout(location = 0) in vec4 Position;

uniform mat4 ProjectionViewModel;
uniform vec4 Color;

out vec4 ParticleColor;


void main()
{
    ParticleColor = Color;
    gl_Position = ProjectionViewModel * Position;
}