#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
uniform vec4 ARGB;

void main() 
{
    FragColor = vec4(ARGB);
}
