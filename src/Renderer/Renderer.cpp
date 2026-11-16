
#include "horizon/Renderer.hpp"

#include <glad/gl.h>

#include <cmath>
#include <iostream>

namespace
{
    constexpr float PI = 3.14159265358979323846f;

    using Matrix4 = float[16];

    void identity(Matrix4 result)
    {
        for (int i = 0; i < 16; ++i)
        {
            result[i] = 0.0f;
        }

        result[0] = 1.0f;
        result[5] = 1.0f;
        result[10] = 1.0f;
        result[15] = 1.0f;
    }

    void multiply(
        const float* a,
        const float* b,
        Matrix4 result)
    {
        Matrix4 temporary{};

        for (int column = 0; column < 4; ++column)
        {
            for (int row = 0; row < 4; ++row)
            {
                for (int k = 0; k < 4; ++k)
                {
                    temporary[column * 4 + row] +=
                        a[k * 4 + row] *
                        b[column * 4 + k];
                }
            }
        }

        for (int i = 0; i < 16; ++i)
        {
            result[i] = temporary[i];
        }
    }

    void makePerspective(
        float fov,
        float aspect,
        float nearPlane,
        float farPlane,
        Matrix4 result)
    {
        for (int i = 0; i < 16; ++i)
        {
            result[i] = 0.0f;
        }

        const float tanHalfFov = std::tan(fov / 2.0f);

        result[0] = 1.0f / (aspect * tanHalfFov);
        result[5] = 1.0f / tanHalfFov;

        result[10] =
            -(farPlane + nearPlane) /
            (farPlane - nearPlane);

        result[11] = -1.0f;

        result[14] =
            -(2.0f * farPlane * nearPlane) /
            (farPlane - nearPlane);
    }

    void makeTranslation(
        float x,
        float y,
        float z,
        Matrix4 result)
    {
        identity(result);

        result[12] = x;
        result[13] = y;
        result[14] = z;
    }

    void makeScale(
        float x,
        float y,
        float z,
        Matrix4 result)
    {
        identity(result);

        result[0] = x;
        result[5] = y;
        result[10] = z;
    }

    void makeRotationX(float angle, Matrix4 result)
    {
        identity(result);

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        result[5] = c;
        result[6] = s;
        result[9] = -s;
        result[10] = c;
    }

    void makeRotationY(float angle, Matrix4 result)
    {
        identity(result);

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        result[0] = c;
        result[2] = -s;
        result[8] = s;
        result[10] = c;
    }

    void makeRotationZ(float angle, Matrix4 result)
    {
        identity(result);

        const float c = std::cos(angle);
        const float s = std::sin(angle);

        result[0] = c;
        result[1] = s;
        result[4] = -s;
        result[5] = c;
    }

    void makeModelMatrix(
        const horizon::Transform& transform,
        Matrix4 result)
    {
        Matrix4 translation{};
        Matrix4 rotationX{};
        Matrix4 rotationY{};
        Matrix4 rotationZ{};
        Matrix4 scale{};

        Matrix4 temporary1{};
        Matrix4 temporary2{};
        Matrix4 temporary3{};

        constexpr float degreesToRadians = PI / 180.0f;

        makeTranslation(
            transform.position.x,
            transform.position.y,
            transform.position.z,
            translation
        );

        makeRotationX(
            transform.rotation.x * degreesToRadians,
            rotationX
        );

        makeRotationY(
            transform.rotation.y * degreesToRadians,
            rotationY
        );

        makeRotationZ(
            transform.rotation.z * degreesToRadians,
            rotationZ
        );

        makeScale(
            transform.scale.x,
            transform.scale.y,
            transform.scale.z,
            scale
        );

        // Model = Translation * RotationZ * RotationY
        //         * RotationX * Scale.

        multiply(rotationX, scale, temporary1);
        multiply(rotationY, temporary1, temporary2);
        multiply(rotationZ, temporary2, temporary3);
        multiply(translation, temporary3, result);
    }
}

namespace horizon
{
    Renderer::Renderer()
        : initialized(false),
          shader(nullptr)
    {
    }

    Renderer::~Renderer()
    {
        destroy();
    }

    bool Renderer::create()
    {
        if (initialized)
        {
            return true;
        }

        const std::string vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPosition;

            uniform mat4 uMVP;

            void main()
            {
                gl_Position = uMVP * vec4(aPosition, 1.0);
            }
        )";

        const std::string fragmentShaderSource = R"(
            #version 330 core

            uniform vec3 uTint;

            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(uTint, 1.0);
            }
        )";

        shader = new Shader();

        if (!shader->create(
            vertexShaderSource,
            fragmentShaderSource))
        {
            delete shader;
            shader = nullptr;

            return false;
        }

        glEnable(GL_DEPTH_TEST);

        initialized = true;

        std::cout
            << "Horizon Renderer initialized successfully!\n";

        return true;
    }

    void Renderer::render(
        const Camera& camera,
        const World& world,
        int width,
        int height)
    {
        if (!initialized ||
            shader == nullptr ||
            width <= 0 ||
            height <= 0)
        {
            return;
        }

        glViewport(0, 0, width, height);

        glClearColor(
            0.025f,
            0.035f,
            0.075f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        Matrix4 projection{};
        Matrix4 viewProjection{};

        const float* view = camera.getViewMatrix();

        makePerspective(
            camera.getZoom() * PI / 180.0f,
            static_cast<float>(width) /
                static_cast<float>(height),
            0.1f,
            100.0f,
            projection
        );

        // ViewProjection = Projection * View.
        multiply(projection, view, viewProjection);

        shader->use();

        // Draw every GameObject in the World.
        for (const auto& object : world.getObjects())
        {
            if (!object)
            {
                continue;
            }

            // Get the object's Mesh.
            std::shared_ptr<Mesh> mesh = object->getMesh();

            if (!mesh || mesh->getVertexCount() == 0)
            {
                continue;
            }

            // Find an existing GPU copy of this Mesh.
            auto gpuIterator = meshCache.find(mesh);

            // Upload the Mesh only if it isn't cached.
            if (gpuIterator == meshCache.end())
            {
                MeshGPU gpuMesh;

                glGenVertexArrays(1, &gpuMesh.VAO);
                glGenBuffers(1, &gpuMesh.VBO);

                glBindVertexArray(gpuMesh.VAO);

                glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);

                const auto& vertices = mesh->getVertices();

                glBufferData(
                    GL_ARRAY_BUFFER,
                    static_cast<GLsizeiptr>(
                        vertices.size() * sizeof(float)
                    ),
                    vertices.data(),
                    GL_STATIC_DRAW
                );

                glVertexAttribPointer(
                    0,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    3 * sizeof(float),
                    nullptr
                );

                glEnableVertexAttribArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

                gpuMesh.vertexCount =
                    static_cast<int>(mesh->getVertexCount());

                gpuIterator = meshCache.emplace(
                    mesh,
                    gpuMesh
                ).first;

                std::cout
                    << "GPU Mesh uploaded: "
                    << gpuMesh.vertexCount
                    << " vertices.\n";
            }

            const MeshGPU& gpuMesh = gpuIterator->second;

            // Calculate the object's model matrix.
            Matrix4 model{};
            Matrix4 mvp{};

            makeModelMatrix(
                object->getTransform(),
                model
            );

            // MVP = Projection * View * Model.
            multiply(viewProjection, model, mvp);

            shader->setMat4("uMVP", mvp);

            // Apply this object's Material color.
            const Vec3& color = object->getMaterial().color;

            shader->setVec3(
                "uTint",
                color.x,
                color.y,
                color.z
            );

            // Draw the Mesh.
            glBindVertexArray(gpuMesh.VAO);

            glDrawArrays(
                GL_TRIANGLES,
                0,
                gpuMesh.vertexCount
            );
        }

        glBindVertexArray(0);
    }

    void Renderer::destroy()
    {
        // Release cached GPU Meshes.
        for (auto& entry : meshCache)
        {
            MeshGPU& gpuMesh = entry.second;

            if (gpuMesh.VBO != 0)
            {
                glDeleteBuffers(1, &gpuMesh.VBO);
            }

            if (gpuMesh.VAO != 0)
            {
                glDeleteVertexArrays(1, &gpuMesh.VAO);
            }
        }

        meshCache.clear();

        delete shader;
        shader = nullptr;

        initialized = false;
    }
}
