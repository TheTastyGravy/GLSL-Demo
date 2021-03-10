// a simple shader
#version 410

layout(location = 0) in vec4 Position;

uniform mat4 ProjectionViewModel;
uniform vec4 MeshFlatColor;
out vec4 simpleFlatColor;


void main()
{
    simpleFlatColor = (MeshFlatColor != 0) ? MeshFlatColor : vec4(1, 0, 1, 1);

    gl_Position = ProjectionViewModel * Position;
}