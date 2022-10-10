#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinates;

out vec3 fragmentPosition;
out vec3 normal;
out vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragmentPosition = vec3(model * vec4(inPosition, 1.0f));
    normal = mat3(transpose(inverse(model))) * inNormal;
    textureCoordinates = inTextureCoordinates;

    gl_Position = projection * view * vec4(fragmentPosition, 1.0f);
}
