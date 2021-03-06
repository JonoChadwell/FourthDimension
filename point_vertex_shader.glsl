#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec4 Pcolor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 Camera;

out vec4 partCol;

void main() { 

	// Billboarding: set the upper 3x3 to be the identity matrix
	mat4 MV0 = V * M;
	MV0[0] = vec4(1.0, 0.0, 0.0, 0.0);
	MV0[1] = vec4(0.0, 1.0, 0.0, 0.0);
	MV0[2] = vec4(0.0, 0.0, 1.0, 0.0);
	gl_Position = P * MV0 * vec4(vertPos.xyz, 1.0);
    gl_PointSize = 1000 / length(gl_Position);
	partCol = Pcolor;
}
