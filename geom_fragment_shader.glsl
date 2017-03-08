#version 330 core

in vec3 fragWorld;
in vec4 fragNormal;
in vec3 fragNormalWorld;

out vec4 color;

float max4(vec4 v)
{
    return max(max(v.x, v.y), max(v.z, v.w));
}

float edginess(vec4 v)
{
    return max(max(v.x + v.y, v.y + v.z), max(v.z + v.w, v.w + v.x));
}

void main()
{
    vec4 normal = normalize(fragNormal);
    if (edginess(fragNormal) < 0.8) {
        discard;
    } else {
        float m = edginess(fragNormal);
        color = vec4(m, m, m, 1.0f);
    }
}
