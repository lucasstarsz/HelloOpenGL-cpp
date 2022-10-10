#version 330 core

out vec4 fragmentColor;

uniform float brightness;

void main()
{
    fragmentColor = vec4(vec3(brightness), 1.0f);
}