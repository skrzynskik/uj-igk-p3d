#version 410

in vec3 vertex_color;
layout(location=0) out vec4 vFragColor;

layout(std140) uniform Color {
    vec4  Kd;
    bool use_map_Kd;
};

void main() {
    vFragColor = vec4(vertex_color, 1.0);
}
