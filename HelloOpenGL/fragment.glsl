#version 330 core

in vec3 color;
out vec4 fragmentColor;

uniform float accumulatedTime;

vec3 shiftColor(vec3 color)
{
    float newX = color.x;
    float newY = color.y;
    float newZ = color.z;

    float modTime = accumulatedTime;
    float m = 0;

    float shiftedX = 0;
    float shiftedY = 0;
    float shiftedZ = 0;

    while (modTime > 0.0f)
    {
        shiftedX = newX;
        shiftedY = newY;
        shiftedZ = newZ;
        
        m = min(modTime, 1.0f);
        newY = (shiftedX * m) + (shiftedY * (1 - m));
        modTime -= m;

        m = min(modTime, 1.0f);
        newZ = (shiftedY * m) + (shiftedZ * (1 - m));
        modTime -= m;

        m = min(modTime, 1.0f);
        newX = (shiftedZ * m) + (shiftedX * (1 - m));
        modTime -= m;
    }

    return vec3(newX, newY, newZ);
}

void main()
{
    fragmentColor = vec4(shiftColor(color), 1.0f);
}
