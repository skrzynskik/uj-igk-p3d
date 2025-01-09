#version 410

layout(location=0) in vec3 a_vertex_position;
layout(location=1) in vec3 a_vertex_color;

out vec3 v_color;

void main() {
    gl_Position = vec4(a_vertex_position, 1.0);
    v_color = a_vertex_color;
}