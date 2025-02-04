#include "Line.h"

Line* Line::line_Instance = nullptr;

Line::Line()
{
    shaderPtr = ResourceManager::getInstance()->lineShader;

    transformPtr = new Transform();

    transformPtr->setScale(1.f, 1.f, 1.f);

    // For a line, we just need two vertices
    vertices = new std::vector<float>
    {
        0.0f, 0.0f, 0.0f,  // start point
        1.f, 0.0f, 0.0f    // end point
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Line::Render(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec4 color)
{
    glUseProgram(shaderPtr->ID);

    // Local Space: Transform (Model)
    globalTransform = glm::mat4(1.f); // For later implementation of children/father
    globalTransform = glm::scale(globalTransform, transformPtr->scale); // Scale
    glm::quat quaternion = glm::quat(glm::radians(transformPtr->rotation));
    glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);
    globalTransform = globalTransform * rotationMatrix; // Rotate
    globalTransform = glm::translate(globalTransform, transformPtr->position); // Translate

    // New vertices from NDC->Screen 
    glm::vec3 startPosScreen = MapToScreen(startPosition);
    glm::vec3 endPosScreen = MapToScreen(endPosition);

    // Create the new list
    vertices = new std::vector<float>
    {
        startPosScreen.x, startPosScreen.y, 0.f,  // Start point
        endPosScreen.x, endPosScreen.y, 0.f         // End point
    };

    // Update VBO with the new vertices (line start and end)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);

    updateProjection();
    //glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), 0.0f, static_cast<float>(SCREEN_HEIGHT));  // Orthographic projection (2D)
    glm::mat4 view = glm::mat4(1.0f);  // Assume identity matrix if camera is static (no rotation/translation)

    MVP = projectionMatrix * view * globalTransform;
    shaderPtr->setMat4("MVP", MVP);
    shaderPtr->setVec4("ARGB", color);

    // Draw Line
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

    delete vertices;
}

glm::vec3 Line::MapToScreen(const glm::vec3& position)
{
    // Apply MVP transformation (model, view, projection)
    glm::vec4 clipSpacePos = MVP * glm::vec4(position, 1.0f); // Transform world position to clip space

    // Perform perspective division to get normalized device coordinates (NDC)
    glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

    // Convert from NDC to screen space (assuming top-left origin)
    glm::vec3 screenPos;
    screenPos.x = (ndcPos.x + 1.0f) * 0.5f * SCREEN_WIDTH;   // NDC to screen x (0 to SCREEN_WIDTH)
    screenPos.y = (1.0f - ndcPos.y) * 0.5f * SCREEN_HEIGHT;  // NDC to screen y (0 to SCREEN_HEIGHT)
    screenPos.z = 0.f; // Assuming 2D line, set z to 0

    return screenPos;
}
