#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "math_util.hpp"

typedef struct GameObjectStruct
{
private:


public:
    glm::vec3 pos;
    glm::vec3 rot;

    GameObjectStruct(glm::vec3 pPos, glm::vec3 pRot)
    {
        pos = pPos;
        rot = pRot;
    }

    GameObjectStruct() : GameObjectStruct(VEC3_ZERO, VEC3_ZERO)
    {

    }

    glm::mat<4, 4, float> getModelMtx() const
    {
        auto posMat = translate(MAT4_IDENTITY, pos);

        glm::mat<4, 4, float> model = glm::rotate(posMat, glm::radians(rot.x), VEC3_RIGHT);
        model                       = glm::rotate(posMat, glm::radians(rot.y), VEC3_UP);
        model                       = glm::rotate(posMat, glm::radians(rot.z), VEC3_FWD);

        return model;
    }
} GameObject;

typedef GameObject GO;
