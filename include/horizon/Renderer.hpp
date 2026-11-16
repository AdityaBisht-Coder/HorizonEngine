
#pragma once

#include "horizon/Shader.hpp"
#include "horizon/Camera.hpp"
#include "horizon/Scene/World.hpp"
#include "horizon/Scene/Mesh.hpp"

#include <memory>
#include <unordered_map>

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

        void render(
            const Camera& camera,
            const World& world,
            int width,
            int height
        );

        void destroy();

    private:
        struct MeshGPU
        {
            unsigned int VAO = 0;
            unsigned int VBO = 0;
            int vertexCount = 0;
        };

        bool initialized;
        Shader* shader;

        std::unordered_map<
            std::shared_ptr<Mesh>,
            MeshGPU
        > meshCache;
    };
}
