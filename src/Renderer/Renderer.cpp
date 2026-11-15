
#include "horizon/Renderer.hpp"

#include <glad/gl.h>

#include <cmath>
#include <iostream>

namespace
{
    constexpr float PI = 3.14159265358979323846f;

    using Matrix4 = float[16];

    // Create an identity matrix.
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

    // Multiply two column-major 4x4 matrices.
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

    // Rotation around the X axis.
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

    // Rotation around the Y axis.
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

    // Translation matrix.
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

    // Perspective projection matrix.
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
}

namespace horizon
{
    Renderer::Renderer()
        : VAO(0),
          VBO(0),
          initialized(false),
          shader(nullptr)
    {
    }

    Renderer::~Renderer()
    {
        destroy();
    }

    // Initialize the renderer.
    bool Renderer::create()
    {
        if (initialized)
        {
            return true;
        }

        // Vertex shader.
        const std::string vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPosition;

            uniform mat4 uMVP;

            void main()
            {
                gl_Position = uMVP * vec4(aPosition, 1.0);
            }
        )";

        // Fragment shader.
        const std::string fragmentShaderSource = R"(
            #version 330 core

            uniform vec3 uTint;

            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(uTint, 1.0);
            }
        )";

        // Create the shader program.
        shader = new Shader();

        if (!shader->create(
            vertexShaderSource,
            fragmentShaderSource))
        {
            delete shader;
            shader = nullptr;

            return false;
        }

        // Cube geometry: 36 vertices, 6 faces.
        const float vertices[] =
        {
            // Front face
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            // Back face
             0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,

            // Left face
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            // Right face
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,

            // Top face
            -0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f,  0.5f,

            // Bottom face
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f
        };

        // Create GPU vertex resources.
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // Position attribute.
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

        // Enable depth testing.
        glEnable(GL_DEPTH_TEST);

        initialized = true;

        std::cout
            << "Horizon Renderer initialized successfully!\n";

        return true;
    }

    // Render the rotating cube.
    void Renderer::render(
        float timeSeconds,
        int width,
        int height)
    {
        if (!initialized || width <= 0 || height <= 0)
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

        Matrix4 rotationX{};
        Matrix4 rotationY{};
        Matrix4 translation{};
        Matrix4 model{};
        Matrix4 view{};
        Matrix4 projection{};
        Matrix4 temporary{};
        Matrix4 mvp{};

        // Calculate cube rotation.
        makeRotationX(timeSeconds * 0.7f, rotationX);
        makeRotationY(timeSeconds, rotationY);

        // Position cube in front of camera.
        makeTranslation(0.0f, 0.0f, -3.0f, translation);

        // Model = Translation * RotationY * RotationX.
        multiply(rotationY, rotationX, model);
        multiply(translation, model, temporary);

        // Identity camera view matrix.
        identity(view);

        // Perspective projection.
        makePerspective(
            45.0f * PI / 180.0f,
            static_cast<float>(width) /
                static_cast<float>(height),
            0.1f,
            100.0f,
            projection
        );

        // MVP = Projection * View * Model.
        multiply(view, temporary, model);
        multiply(projection, model, mvp);

        // Activate shader.
        shader->use();

        // Send uniforms to GPU.
        shader->setMat4("uMVP", mvp);
        shader->setVec3(
            "uTint",
            0.15f,
            0.55f,
            1.0f
        );

        // Draw cube.
        glBindVertexArray(VAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );

        glBindVertexArray(0);
    }

    // Release GPU resources.
    void Renderer::destroy()
    {
        if (VBO != 0)
        {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }

        if (VAO != 0)
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }

        delete shader;
        shader = nullptr;

        initialized = false;
    }
}
