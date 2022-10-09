#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTextureCoordinate;

out vec2 texCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * (transform * vec4(inPos, 1.0f));
    texCoord = inTextureCoordinate;
}
