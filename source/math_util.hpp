#pragma once

#include <constants.hpp>

glm::mat4 rotate(glm::mat4 pMtx, float pPitchRad, float pYawRad, float pRollRad)
{
    return glm::rotate(pMtx, TAU_F, glm::vec3(pPitchRad / TAU_F, pYawRad / TAU_F, pRollRad / TAU_F));
}
