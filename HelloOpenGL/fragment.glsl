#version 330 core
out vec4 fragmentColor;
uniform vec4 uniformColor;

void main()
{
    fragmentColor = uniformColor;
}
