#version 330 core

uniform float sliceWidth;
uniform float sliceOffset;
uniform float wireframeWidth;

in float wPart;
in vec3 fragWorld;
in vec4 fragSide;

out vec4 color;

void main()
{
    if (sliceWidth < abs(wPart + sliceOffset))
    {
        discard;
    }
    else
    {
        float c = (((wPart + sliceOffset) / sliceWidth) + 1) / 2;
        color = vec4(c, c, c, 1.0f);
    }
}
