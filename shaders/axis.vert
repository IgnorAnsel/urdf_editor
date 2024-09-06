#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 ourColor;
out vec3 fragColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(void)
{
    gl_Position = projection*view*model*vec4(aPos, 1.0);
    fragColor = ourColor;
}
