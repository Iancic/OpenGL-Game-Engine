#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

uniform float index_column;
uniform float index_row;
uniform float uv_size_x;
uniform float uv_size_y;

void main()
{
    vec2 uvCoords = vec2(TexCoords.x * uv_size_x, TexCoords.y * uv_size_y) + vec2(uv_size_x * index_column, uv_size_y * index_row);
    color = vec4(spriteColor, 1.0) * texture(image, uvCoords);
}