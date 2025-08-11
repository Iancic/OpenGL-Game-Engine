#pragma once
#include <glad/glad.h>

struct SpriteInfo
{
    int SpriteRow = 0;    // Sprite location in the sprite sheet - row
    int SpriteCol = 0;    // Sprite location in the sprite sheet - col
    int SpriteWidth = 0;
};

class Texture2D
{
public:
    Texture2D();
    unsigned int ID;
    unsigned int Width, Height; // width and height of loaded image in pixels

    unsigned int Internal_Format; // format of texture object
    unsigned int Image_Format; // format of loaded image

    unsigned int Wrap_S; // wrapping mode on S axis
    unsigned int Wrap_T; // wrapping mode on T axis
    unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
    unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels

    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    void Bind() const;
};

