#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentPosition;
out vec3 normal;

void main()
{
    fragmentPosition = vec3(model * vec4(inPos, 1.0f));
    normal = mat3(transpose(inverse(model))) * inNormal;

    gl_Position = projection * view * vec4(fragmentPosition, 1.0f);
}
