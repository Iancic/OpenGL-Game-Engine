#pragma once
#include "precomp.h"

#include "Shader.hpp"
#include "Texture2D.hpp"
#include "ResourceManager.hpp"
#include "Camera.hpp"

class SpriteRenderer
{
public:
    SpriteRenderer();
    ~SpriteRenderer();

    void DrawSprite(Camera* activeCamera, Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    unsigned int quadVAO;
private:
    Shader* shader;

    void initRenderData();

};

