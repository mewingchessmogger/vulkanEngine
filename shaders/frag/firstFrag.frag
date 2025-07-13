#version 450

layout(location = 0) in vec3 fragColor;
layout (location = 0) out vec4 outColor;

//1.0f,0.0f,0.5f
void main() {
	outColor = vec4(fragColor,1.0f);
}