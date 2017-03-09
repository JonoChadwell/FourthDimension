#version 330 core

uniform int renderMode;
uniform float sliceWidth;

in float wPart;
in vec3 fragWorld;
in vec4 fragSide;

out vec4 color;

float max4(vec4 v)
{
    return max(max(v.x, v.y), max(v.z, v.w));
}

float edginessTri(vec4 v)
{
    return max(max(v.x + v.y, v.y + v.z), v.z + v.x);
}

float edginessQuad(vec4 v)
{
    return max(max(v.x + v.y, v.y + v.z), max(v.z + v.w, v.w + v.x));
}

void main()
{
    float en = 1;
    if (renderMode == 1)
    {
        en = edginessTri(fragSide);
    }
    if (renderMode == 2)
    {
        en = edginessQuad(fragSide);
    }
    
    if (wPart > sliceWidth || wPart < -sliceWidth) 
    {
        discard;
    }
    
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
