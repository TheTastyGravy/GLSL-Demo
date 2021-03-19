// a simple shader
#version 410

out vec4 FragColor;

in vec4 ParticleColor;


void main()
{
    FragColor = ParticleColor;
}