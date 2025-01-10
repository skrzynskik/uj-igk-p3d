#version 410

in vec3 vertex_color;
layout(location=0) out vec4 vFragColor;

layout(std140) uniform Modifier {
    float strength;
    vec3 color;
};

void main() {
    vec3 modified_color = vertex_color * color * strength;
    vFragColor = vec4(modified_color, 1.0);
}