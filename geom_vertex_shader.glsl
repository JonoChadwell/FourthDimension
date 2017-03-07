#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec4 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 Q;
uniform mat4 R;
uniform vec4 objPos;

out vec3 fragWorld;
out vec3 fragNormal;
out vec3 fragNormalWorld;

void main()
{
    vec4 fourthDimPos = R * vertPos + objPos;
    vec4 projectedPos = Q * fourthDimPos;
    vec4 worldPos = M * vec4(projectedPos.xyz, 1);
    gl_Position = P * V * M * vec4(projectedPos.xyz, 1);
    
    fragWorld = worldPos.xyz;
    vec4 projectedNormal = Q * R * vertNor;
    fragNormal = vertNor.xyz;
    fragNormalWorld = (M * vec4(projectedNormal.xyz, 0)).xyz;
}
