
#pragma once

#include <string>

namespace horizon
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        bool create(
            const std::string& vertexSource,
            const std::string& fragmentSource
        );

        void use() const;
        void destroy();

        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setMat4(const std::string& name, const float* value) const;
        void setVec3(
            const std::string& name,
            float x,
            float y,
            float z
        ) const;

        unsigned int id() const;

    private:
        unsigned int programId;

        unsigned int compileShader(
            unsigned int type,
            const std::string& source
        );

        int getUniformLocation(const std::string& name) const;
    };
}
