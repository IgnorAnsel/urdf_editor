#version 450 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 IDColor;
in vec3 ourColor;
in vec2 TexCord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D textureHuaji;
uniform float ratio;
uniform bool isHaveTextue;
uniform int modelID;

void main()
{
        FragColor = vec4(ourColor, 1.0f);
        IDColor = vec4(float(modelID) / 255.0, 0.0, 0.0, 1.0);
}
