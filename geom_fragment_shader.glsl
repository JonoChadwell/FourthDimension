#version 330 core
in vec3 fragWorld;
in vec3 fragNormal;
in vec3 fragNormalWorld;
out vec4 color;

void main()
{
    vec3 normal = normalize(fragNormal);
    color = vec4(-normal, 0.3);
}
