#version 410

layout(location = 0) in vec4 a_vertex_position;
layout(location = 1) in vec3 a_vertex_color;

out vec3 vertex_color;

layout(std140) uniform Transformations {
    vec2 scale;
    vec2 translation;
    mat2 rotation;
};

void main() {
    gl_Position.xy = rotation * (scale * a_vertex_position.xy) + translation;
    gl_Position.zw = a_vertex_position.zw;
    vertex_color = a_vertex_color;
}