#version 410

layout(location=0) in vec3 a_vertex_position;
layout(location=1) in vec3 a_vertex_color;

out vec3 v_color;

layout(std140) uniform Transformations {
    vec2 scale;
    vec2 translation;
    mat2 rotation;
};

void main() {
    gl_Position = vec4(a_vertex_position, 1.0);
    gl_Position.xy = rotation*(scale*a_vertex_position.xy)+translation;
    gl_Position.zw = a_vertex_position.zw; 
    v_color = a_vertex_color;
}