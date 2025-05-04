#version 330 core

in vec4 vertexColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform float time;

uniform vec2 offset;

uniform vec3 primaryColor;
uniform vec3 secondaryColor;

uniform float zoomFactor;
uniform float motionSpeed;
uniform float rotationSpeed;

uniform float spotSize;

vec2 randomGradient(vec2 pos)
{
    float angle = fract(sin(dot(pos ,vec2(127.1, 311.7))) * 43758.5453 + time * rotationSpeed) * 6.28318;
    return vec2(cos(angle), sin(angle));
}

float dotGridGradient(vec2 integerPos, vec2 pos)
{
    vec2 gradient = randomGradient(integerPos);
    vec2 diff = pos - integerPos;
    return dot(gradient, diff);
}

float perlinNoise(vec2 pos)
{
    vec2 i0 = floor(pos);
    vec2 i1 = i0 + vec2(1.0, 0.0);
    vec2 j1 = i0 + vec2(0.0, 1.0);
    vec2 ij = i0 + vec2(1.0, 1.0);

    vec2 f = fract(pos);
    vec2 u = f * f * (3.0 - 2.0 * f); // smoothstep

    float a = dotGridGradient(i0, pos);
    float b = dotGridGradient(i1, pos);
    float c = dotGridGradient(j1, pos);
    float d = dotGridGradient(ij, pos);

    float x1 = mix(a, b, u.x);
    float x2 = mix(c, d, u.x);
    return mix(x1, x2, u.y);
}

void main()
{
    // Perlin Noise
    // Animate Noise
    vec2 uv = TexCoord * offset;
    uv += vec2(sin(time), cos(time)) * motionSpeed; // Motion Speed Affects How Dynamic Noise Is
    float noise = perlinNoise(uv * zoomFactor); // Generate noise then clamp it from 0.2 to 1.0

    // Threshold the noise
    vec3 value = noise > spotSize ? primaryColor : secondaryColor;

    // Output black or white
    FragColor = vec4(value, 1.0);
}