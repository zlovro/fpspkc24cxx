#pragma once

#include <globals.hpp>
#include "game_object.hpp"

typedef struct CameraStruct : GO
{
private:


public:
    float yaw = 0, pitch = 0;
    float fov = 75;

    glm::mat4 view, projection;

    void calculateMtxs()
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        view = lookAt(pos, pos + normalize(direction), pos);

        int width = 0, height = 0;
        glfwGetFramebufferSize(gGameWnd, &width, &height);
        projection = glm::perspective(glm::radians(fov), (float)width / height, 0.1F, 10000.0F);
    }
} Camera;
