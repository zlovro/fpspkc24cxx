#pragma once

const char* SHADER_FRAG_BASIC = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 1.0F, 1.0F, 1.0F);
}
)";