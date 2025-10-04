#pragma once
#include "precomp.h"

#include "Shader.hpp"
#include "Texture2D.hpp"
#include "Animation.hpp"
#include "ResourceManager.hpp"
#include "Camera.hpp"

class SpriteRenderer
{
public:
    SpriteRenderer();
    ~SpriteRenderer();

    unsigned int quadVAO;

    void DrawSprite(Camera& activeCamera, Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawIndexedSprite(Animation& animation, Camera& activeCamera, Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    
private:
    Shader* spriteShader;
    Shader* animationShader;

    void initRenderData();

};

