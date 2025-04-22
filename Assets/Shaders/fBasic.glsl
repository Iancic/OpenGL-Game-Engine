#version 330 core

out vec4 FragColor;

uniform vec4 ARGB;

void main() 
{
    FragColor = vec4(ARGB);  // Output white color
}
