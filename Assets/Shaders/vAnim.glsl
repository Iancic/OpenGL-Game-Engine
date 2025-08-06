#version 330

layout(location = 0) in vec3 Position;

uniform mat4 gWVP;
uniform float gScale;

out vec2 TexCoord0;

layout(std140) uniform QuadInfo
{
    vec2 BasePos[256];
    vec2 WidthHeight[256];
    vec2 TexCoords[256];
    vec2 TexWidthHeight[256];
};

uniform int QuadIndex; // You will pass the index of the current quad here

void main()
{
    vec2 basePos = BasePos[QuadIndex];
    vec2 size = WidthHeight[QuadIndex] * gScale;
    vec2 texCoordBase = TexCoords[QuadIndex];
    vec2 texSize = TexWidthHeight[QuadIndex];

    vec2 pos = basePos + Position.xy * size;

    gl_Position = gWVP * vec4(pos, Position.z, 1.0);

    TexCoord0 = texCoordBase + (Position.xy + vec2(0.5)) * texSize;
}