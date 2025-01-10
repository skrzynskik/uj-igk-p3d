#version 410

layout(location = 0) in vec4 a_vertex_position;
layout(location = 1) in vec3 a_vertex_color;

out vec3 vertex_color;

layout(std140) uniform Transformations {
    mat4 PVM;
};

void main() {
	gl_Position = PVM * a_vertex_position;
    vertex_color = a_vertex_color;
}