#version 410 core

layout(location = 0) in vec2 vPosition; 
layout(location = 1) in vec2 vTexCoords; 

out vec2 fTexCoords;

void main()
{
    fTexCoords = vTexCoords;
    gl_Position = vec4(vPosition * 2.0 - 1.0, 0.0, 1.0);
}
