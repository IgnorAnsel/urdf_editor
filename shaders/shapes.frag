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
        float r = float((modelID & 0xFF0000) >> 16) / 255.0;
        float g = float((modelID & 0x00FF00) >> 8) / 255.0;
        float b = float(modelID & 0x0000FF) / 255.0;

        IDColor = vec4(r, g, b, 1.0);
}
