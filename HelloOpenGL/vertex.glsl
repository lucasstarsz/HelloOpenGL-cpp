#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoordinate;

out vec3 color;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(inPos, 1.0f);
    color = inColor;
    texCoord = inTextureCoordinate;
}
