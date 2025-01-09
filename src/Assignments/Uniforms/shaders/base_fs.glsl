#version 410

in vec3 v_color;
out vec4 FragColor;

layout(std140) uniform Modifier {
    float strength;
    vec3  color; 
};

void main() {
    FragColor = vec4(v_color, 1.0);
}