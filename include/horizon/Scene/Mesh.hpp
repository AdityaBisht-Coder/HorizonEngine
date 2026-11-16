
#pragma once

#include <vector>
#include <memory>
#include <cstddef>

namespace horizon
{
    class Mesh
    {
    public:
        explicit Mesh(const std::vector<float>& vertices);

        const std::vector<float>& getVertices() const;

        std::size_t getVertexCount() const;

        static std::shared_ptr<Mesh> createCube();

    private:
        std::vector<float> vertices;
    };
}
