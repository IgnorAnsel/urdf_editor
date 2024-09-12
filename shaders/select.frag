#version 450 core
in vec3 FragPos;
in vec3 Normal;
in vec3 ViewDir;

out vec4 FragColor;
layout(location = 1) out vec4 IDColor;

uniform vec3 objectColor;
uniform bool isSelected;
uniform int modelID;

void main()
{
    float edgeIntensity = dot(normalize(Normal), normalize(ViewDir));
    float r = float((modelID & 0xFF0000) >> 16) / 255.0;
    float g = float((modelID & 0x00FF00) >> 8) / 255.0;
    float b = float(modelID & 0x0000FF) / 255.0;

    IDColor = vec4(r, g, b, 1.0);
    if (isSelected && abs(edgeIntensity) < 0.2)
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else
    {
        FragColor = vec4(objectColor, 1.0);
    }
}
