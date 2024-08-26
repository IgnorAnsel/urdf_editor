#version 450 core

in vec3 FragColor;

out vec4 finalColor;

void main()
{
    finalColor = vec4(FragColor, 1.0);
}
