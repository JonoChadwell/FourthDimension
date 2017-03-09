#version 330 core

uniform float sliceWidth;
uniform float sliceOffset;

in float wPart;
in vec3 fragWorld;
in vec4 fragSide;

out vec4 color;

float edginessTri(vec4 v)
{
    return max(max(v.x + v.y, v.y + v.z), v.z + v.x);
}

void main()
{
    if (sliceWidth < abs(wPart + sliceOffset)) 
    {
        discard;
    }

    float en = edginessTri(fragSide);
    if (en < 0.85)
    {
        discard;
    }
    else
    {
        float c = 1 - (1 - en) * 3;
        color = vec4(c, c, c, 1.0f);
    }
}
