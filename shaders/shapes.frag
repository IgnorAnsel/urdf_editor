#version 450 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D textureHuaji;
uniform float ratio;
uniform bool isHaveTextue;
void main()
{
        FragColor = vec4(ourColor, 1.0f);
}
