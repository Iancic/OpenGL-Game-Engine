#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 MVP;

out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = MVP * vec4(aPosition, 1.0);
}