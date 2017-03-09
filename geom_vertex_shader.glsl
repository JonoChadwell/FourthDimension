#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec4 vertSide;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 Q;
uniform mat4 R;
uniform int divW;
uniform vec4 objPos;

out vec3 fragWorld;
out vec4 fragSide;
out float wPart;
// out vec3 fragNormalWorld;

void main()
{
    vec4 fourthDimPos = R * vertPos + objPos;
    vec4 projectedPos = Q * fourthDimPos;
    wPart = projectedPos.w;
    vec4 worldPos;
    if (divW == 0)
    {
        vec4 worldPos = M * vec4(projectedPos.xyz, 1);
        gl_Position = P * V * M * vec4(projectedPos.xyz, 1);
    }
    else
    {
        vec4 worldPos = M * vec4(projectedPos.xyz / (wPart + 20) * 20, 1);
        gl_Position = P * V * M * vec4(projectedPos.xyz / (wPart + 20) * 20, 1);
    }
    
    fragWorld = worldPos.xyz;
    // vec4 projectedNormal = Q * R * vertNor;
    fragSide = vertSide;
    // fragNormalWorld = (M * vec4(projectedNormal.xyz, 0)).xyz;
}
