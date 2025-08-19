#pragma once
#include "precomp.h"

struct TransformComponent
{
    glm::mat4 LocalMatrix = glm::mat4(1.0f);  // Built from TRS
    glm::mat4 WorldMatrix = glm::mat4(1.0f);  // Calculated per frame

    void SetTranslation(glm::mat4& matrix, const glm::vec3& translation)
    {
        matrix[3][0] = translation.x;
        matrix[3][1] = translation.y;
        matrix[3][2] = translation.z;
    }

    void SetRotation(glm::mat4& matrix, const glm::vec3& eulerRadians)
    {
        // Build new rotation matrix from Euler angles (XYZ: pitch, yaw, roll)
        glm::mat4 rotation =
            glm::rotate(glm::mat4(1.0f), eulerRadians.x, glm::vec3(1, 0, 0)) *  // Pitch
            glm::rotate(glm::mat4(1.0f), eulerRadians.y, glm::vec3(0, 1, 0)) *  // Yaw
            glm::rotate(glm::mat4(1.0f), eulerRadians.z, glm::vec3(0, 0, 1));   // Roll

        // Extract current scale from original matrix
        glm::vec3 scale;
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));

        // Apply new rotation, reapplying original scale
        matrix[0] = glm::vec4(glm::normalize(glm::vec3(rotation[0])) * scale.x, 0.0f);
        matrix[1] = glm::vec4(glm::normalize(glm::vec3(rotation[1])) * scale.y, 0.0f);
        matrix[2] = glm::vec4(glm::normalize(glm::vec3(rotation[2])) * scale.z, 0.0f);
    }

    void SetScale(glm::mat4& matrix, const glm::vec3& scale)
    {
        // Extract rotation matrix (normalized axes)
        glm::vec3 x = glm::normalize(glm::vec3(matrix[0]));
        glm::vec3 y = glm::normalize(glm::vec3(matrix[1]));
        glm::vec3 z = glm::normalize(glm::vec3(matrix[2]));

        matrix[0] = glm::vec4(x * scale.x, 0.0f);
        matrix[1] = glm::vec4(y * scale.y, 0.0f);
        matrix[2] = glm::vec4(z * scale.z, 0.0f);
    }

    glm::quat QuatFromEulerDegrees(const glm::vec3& eulerDegrees)
    {
        glm::vec3 rad = glm::radians(eulerDegrees);
        return glm::angleAxis(rad.z, glm::vec3(0, 0, 1)) *
            glm::angleAxis(rad.y, glm::vec3(0, 1, 0)) *
            glm::angleAxis(rad.x, glm::vec3(1, 0, 0));
    }

    // Recompose transform matrix from translation, rotation (in degrees), and scale
    glm::mat4 ComposeMatrix(const glm::vec3& translation, const glm::vec3& rotationDeg, const glm::vec3& scale)
    {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
        glm::quat Q = QuatFromEulerDegrees(rotationDeg);
        glm::mat4 R = glm::toMat4(Q);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }

    glm::vec3 GetTranslation(const glm::mat4& matrix)
    {
        return glm::vec3(matrix[3]);
    }

    glm::vec3 GetScale(const glm::mat4& matrix)
    {
        glm::vec3 scale;
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));
        return scale;
    }

    glm::vec3 GetRotationEuler(const glm::mat4& matrix)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::vec3 scale;
        glm::quat rotationQuat;
        glm::vec3 translation;

        glm::decompose(matrix, scale, rotationQuat, translation, skew, perspective);
        return glm::eulerAngles(rotationQuat); // Returns radians
    }
};