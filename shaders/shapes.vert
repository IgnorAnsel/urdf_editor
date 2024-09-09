#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCord;
out vec2 TexCord;
uniform mat4 RotationMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(void)
{
    gl_Position = projection*view*model*vec4(aPos, 1.0);
    TexCord = aTexCord;
}
