
#include "horizon/Shader.hpp"

#include <glad/gl.h>

#include <iostream>

namespace horizon
{
    Shader::Shader()
        : programId(0)
    {
    }

    Shader::~Shader()
    {
        destroy();
    }

    unsigned int Shader::compileShader(
        unsigned int type,
        const std::string& source
    )
    {
        unsigned int shader = glCreateShader(type);

        const char* sourceCode = source.c_str();
        glShaderSource(shader, 1, &sourceCode, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char infoLog[1024] = {};
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);

            std::cerr << "Horizon Shader Compilation Error:\n"
                      << infoLog << '\n';

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    bool Shader::create(
        const std::string& vertexSource,
        const std::string& fragmentSource
    )
    {
        destroy();

        unsigned int vertexShader =
            compileShader(GL_VERTEX_SHADER, vertexSource);

        if (vertexShader == 0)
        {
            return false;
        }

        unsigned int fragmentShader =
            compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        if (fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            return false;
        }

        programId = glCreateProgram();

        glAttachShader(programId, vertexShader);
        glAttachShader(programId, fragmentShader);
        glLinkProgram(programId);

        int success = 0;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        if (!success)
        {
            char infoLog[1024] = {};
            glGetProgramInfoLog(
                programId,
                sizeof(infoLog),
                nullptr,
                infoLog
            );

            std::cerr << "Horizon Shader Linking Error:\n"
                      << infoLog << '\n';

            glDeleteProgram(programId);
            programId = 0;

            return false;
        }

        std::cout << "Horizon Shader compiled and linked successfully!\n";
        return true;
    }

    void Shader::use() const
    {
        if (programId != 0)
        {
            glUseProgram(programId);
        }
    }

    void Shader::destroy()
    {
        if (programId != 0)
        {
            glDeleteProgram(programId);
            programId = 0;
        }
    }

    int Shader::getUniformLocation(const std::string& name) const
    {
        if (programId == 0)
        {
            return -1;
        }

        return glGetUniformLocation(programId, name.c_str());
    }

    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(getUniformLocation(name), value);
    }
    void Shader::setMat4(const std::string& name, const float* value) const
{
    glUniformMatrix4fv(
        getUniformLocation(name),
        1,
        GL_FALSE,
        value
    );
}

    void Shader::setVec3(
        const std::string& name,
        float x,
        float y,
        float z
    ) const
    {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    unsigned int Shader::id() const
    {
        return programId;
    }
}
