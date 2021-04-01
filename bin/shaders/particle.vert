// a particle shader with billboarding
#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 ParticlePos;   //position.xyz, scale
layout(location = 2) in vec4 ParticleColor;

uniform vec3 CameraPosition;
uniform mat4 ProjectionViewMatrix;

out vec4 vParticleColor;


void main()
{
    //create billboard transform to face towards the camera
	vec3 zAxis= normalize(ParticlePos.xyz - CameraPosition);
	vec3 xAxis= cross(vec3(0, 1, 0), zAxis);
	vec3 yAxis= cross(zAxis, xAxis);
	mat4 billboard = mat4(vec4(xAxis, 0), vec4(yAxis, 0), vec4(zAxis, 0), vec4(ParticlePos.xyz, 1));

    //scalar matrix to the particles size
    mat4 scale = mat4(ParticlePos.w);
    scale[3][3] = 1;
    //apply transformations to get model matrix
	billboard *= scale;

    vParticleColor = ParticleColor;
    gl_Position = ProjectionViewMatrix * billboard * Position;
}