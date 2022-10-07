#version 330 core

in vec3 color;
in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D tex;
uniform float accumulatedTime;

vec3 shiftColor(vec3 color)
{
    float newX = color.x;
    float newY = color.y;
    float newZ = color.z;

    float modTime = accumulatedTime;
    float colorShiftFactor = 0;

    float shiftedX = 0;
    float shiftedY = 0;
    float shiftedZ = 0;

    while (modTime > 0.0f)
    {
        shiftedX = newX;
        shiftedY = newY;
        shiftedZ = newZ;

        colorShiftFactor = min(modTime, 1.0f);
        newY = (shiftedX * colorShiftFactor) + (shiftedY * (1 - colorShiftFactor));
        modTime -= colorShiftFactor;

        colorShiftFactor = min(modTime, 1.0f);
        newZ = (shiftedY * colorShiftFactor) + (shiftedZ * (1 - colorShiftFactor));
        modTime -= colorShiftFactor;

        colorShiftFactor = min(modTime, 1.0f);
        newX = (shiftedZ * colorShiftFactor) + (shiftedX * (1 - colorShiftFactor));
        modTime -= colorShiftFactor;
    }

    return vec3(newX, newY, newZ);
}

void main()
{
    fragmentColor = mix(texture(tex, texCoord), vec4(shiftColor(color), 1.0f), 0.2f);
}
