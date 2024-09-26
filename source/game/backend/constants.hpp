#pragma once
#include <glm/glm.hpp>

constexpr auto PI  = std::numbers::pi;
constexpr auto TAU = 2 * PI;

constexpr auto PI_F  = (f32)PI;
constexpr auto TAU_F = 2 * PI_F;

constexpr auto VEC3_ZERO    = glm::vec3(0, 0, 0);
constexpr auto VEC3_RIGHT = glm::vec3(1, 0, 0);
constexpr auto VEC3_UP    = glm::vec3(0, 1, 0);
constexpr auto VEC3_FWD   = glm::vec3(0, 0, 1);

constexpr auto MAT4_IDENTITY = glm::mat4(1.0F);
