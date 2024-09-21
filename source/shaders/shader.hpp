#pragma once

#include <format>
#include <glad/glad.h>
#include <error_context.hpp>

typedef struct ShaderStruct
{
private:
    static GLuint compile(const char* pSrc, GLenum pType)
    {
        const auto shaderId = glCreateShader(pType);

        glShaderSource(shaderId, 1, &pSrc, nullptr);
        glCompileShader(shaderId);

        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(shaderId, 1024, nullptr, infoLog);
            gErrCtx.setErr(std::format("Shader compilation failed: {}\nSource:\n{}\n", infoLog, pSrc));
            return -1;
        }

        return shaderId;
    }

    GLint getVarLoc(const char* pName) const
    {
        return glGetUniformLocation(id, pName);
    }

public:
    GLuint id;

    ShaderStruct(const char* pVtxSrc, const char* pFragSrc)
    {
        const auto vtx = compile(pVtxSrc, GL_VERTEX_SHADER);
        if (gErrCtx.isErr())
        {
            return;
        }

        const auto frag = compile(pFragSrc, GL_FRAGMENT_SHADER);
        if (gErrCtx.isErr())
        {
            return;
        }

        id = glCreateProgram();
        glAttachShader(id, vtx);
        glAttachShader(id, frag);
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[0x400];
            glGetProgramInfoLog(id, 0x400, nullptr, infoLog);
            gErrCtx.setErr(infoLog);
            return;
        }

        glDeleteShader(vtx);
        glDeleteShader(frag);
    }

    void setMtx4(const char* pVarName, glm::mat4x4 pValue) const
    {
        glUniformMatrix4fv(getVarLoc(pVarName), 1, false, value_ptr(pValue));
    }

    void use() const
    {
        glUseProgram(id);
    }
} Shader;
