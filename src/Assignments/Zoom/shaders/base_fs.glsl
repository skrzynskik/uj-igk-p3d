#version 420

in vec3 vertex_color;
layout(location=0) out vec4 vFragColor;

layout(std140) uniform Modifier{
	float strength;
	vec3  color;
};

void main() {
    vFragColor = vec4(vertex_color, 1.0);
}
