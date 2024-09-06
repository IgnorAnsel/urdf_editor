#version 450 core

in vec3 fragColor;
out vec4 FragColor;

void main(void)
{
    FragColor = vec4(fragColor, 1.0);
}
