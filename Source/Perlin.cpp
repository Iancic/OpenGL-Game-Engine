#include "Perlin.hpp"

Perlin::Perlin()
{
    NewNoise();
}

// Cubic smoothstep function for interpolation
float Perlin::Smoothstep(float t)
{
    return t * t * (3 - 2 * t);
}

// Cubic Interpolation: Inspired from Youtuber Zipped
// Linear Interpolation: Inspired from Learn OpenGL 
// (https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation)

float Perlin::Interpolate(float a0, float a1, float w, bool isLinear)
{
   if (isLinear)
        return a0 * (1 - w) + a1 * w;
   else
        return (a1 - a0) * (3.f - w * 2.f) * w * w + a0;
}

// Dot product of distance and gradient vectors
float Perlin::DotGridGradient(int ix, int iy, float x, float y)
{
    glm::vec2 gradient = RandomGradient(ix, iy); // Random gradient vector
    glm::vec2 distance(x - ix, y - iy); // Distance vector

    return glm::dot(gradient, distance);
}

// Sample noise at desired position in texture
float Perlin::SampleNoise(float x, float y)
{
    // Find where the sample is located (corners of the box)
    int x0 = static_cast<int>(floor(x));
    int x1 = x0 + 1;
    int y0 = static_cast<int>(floor(y));
    int y1 = y0 + 1;

    float sx = Smoothstep(x - x0);
    float sy = Smoothstep(y - y0);

    // Top Corners:Calculate weight of each corner then interpolate between them
    float n0 = DotGridGradient(x0, y0, x, y);
    float n1 = DotGridGradient(x1, y0, x, y);
    float ix0 = Interpolate(n0, n1, sx, true); // Top Side Weight

    // Bottom Corners: Do the same for them
    n0 = DotGridGradient(x0, y1, x, y);
    n1 = DotGridGradient(x1, y1, x, y);
    float ix1 = Interpolate(n0, n1, sx, true); // Bottom Side Weight

    return Interpolate(ix0, ix1, sy, true); // Interpolate Top and Bottom Side Weight Influence
}

void Perlin::GenerateTexture()
{
    glGenTextures(1, &perlinTexture);
    glBindTexture(GL_TEXTURE_2D, perlinTexture);

    // Specify texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the Perlin noise data into the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, perlinData.data());

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Perlin::NewNoise()
{
    // Delete old texture if it exists
    if (perlinTexture != 0)
    {
        glDeleteTextures(1, &perlinTexture);
        perlinTexture = 0;
    }

    SEED = static_cast<unsigned int>(time(nullptr));
    perlinData = GenerateNoise(width, height, zoomFactor);

    glGenTextures(1, &perlinTexture);
    glBindTexture(GL_TEXTURE_2D, perlinTexture);

    // Specify texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the Perlin noise data into the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, perlinData.data());

    glGenerateMipmap(GL_TEXTURE_2D);
}

std::vector<unsigned char> Perlin::GenerateNoise(int widthArg, int heightArg, float scaleArg)
{
    std::vector<unsigned char> data(widthArg * heightArg * 3); // RGB format

    for (int y = 0; y < heightArg; ++y)
    {
        for (int x = 0; x < widthArg; ++x)
        {
            // Convert to texture size.
            float xf = x / static_cast<float>(widthArg) * scaleArg;
            float yf = y / static_cast<float>(heightArg) * scaleArg;

            float noise = SampleNoise(xf, yf) * 0.5f + 0.5f;

            // Map to [0, 255] for RGB
            unsigned char value = static_cast<unsigned char>(noise * 255);

            int index = (y * width + x) * 3;
            data[index] = static_cast<unsigned char>((1 - noise) * redChannel); // Red
            data[index + 1] = static_cast<unsigned char>((1 - noise) * greenChannel); // Green
            data[index + 2] = static_cast<unsigned char>((1 - noise) * blueChannel);  // Blue
        }
    }

    return data;
}

glm::vec2 Perlin::RandomGradient(int ix, int iy)
{
    unsigned int hash = ix * 374761393u + iy * 668265263u + SEED; // Add the seed

    hash = (hash ^ (hash >> 13u)) * 1274126177u;
    hash = (hash ^ (hash >> 16u));

    float angle = (hash % 360) * 3.14159265359f / 180.0f; // Convert to radians

    return glm::vec2(cos(angle), sin(angle)); // Gradient vector
}