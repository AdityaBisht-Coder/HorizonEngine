
#pragma once

#include "horizon/Shader.hpp"
#include "horizon/Camera.hpp"

namespace horizon
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        bool create();

        void render(const Camera& camera, int width, int height);

        void destroy();

    private:
        unsigned int VAO;
        unsigned int VBO;

        bool initialized;

        Shader* shader;
    };
}
