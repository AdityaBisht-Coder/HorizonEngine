
#include "horizon/Camera.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float PI = 3.14159265358979323846f;

    float toRadians(float degrees)
    {
        return degrees * PI / 180.0f;
    }

    float dot(
        const float* a,
        const float* b)
    {
        return
            a[0] * b[0] +
            a[1] * b[1] +
            a[2] * b[2];
    }

    void cross(
        const float* a,
        const float* b,
        float* result)
    {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }

    void normalize(float* vector)
    {
        const float length = std::sqrt(dot(vector, vector));

        if (length <= 0.000001f)
        {
            return;
        }

        vector[0] /= length;
        vector[1] /= length;
        vector[2] /= length;
    }
}

namespace horizon
{
    Camera::Camera()
        : position{0.0f, 0.0f, 3.0f},
          front{0.0f, 0.0f, -1.0f},
          up{0.0f, 1.0f, 0.0f},
          right{1.0f, 0.0f, 0.0f},
          worldUp{0.0f, 1.0f, 0.0f},
          yaw(-90.0f),
          pitch(0.0f),
          movementSpeed(4.0f),
          mouseSensitivity(0.1f),
          zoom(45.0f),
          viewMatrix{}
    {
        updateVectors();
        updateViewMatrix();
    }

    void Camera::processKeyboard(
        CameraDirection direction,
        float deltaTime)
    {
        const float velocity = movementSpeed * deltaTime;

        switch (direction)
        {
        case CameraDirection::Forward:
            for (int i = 0; i < 3; ++i)
                position[i] += front[i] * velocity;
            break;

        case CameraDirection::Backward:
            for (int i = 0; i < 3; ++i)
                position[i] -= front[i] * velocity;
            break;

        case CameraDirection::Left:
            for (int i = 0; i < 3; ++i)
                position[i] -= right[i] * velocity;
            break;

        case CameraDirection::Right:
            for (int i = 0; i < 3; ++i)
                position[i] += right[i] * velocity;
            break;
        }

        updateViewMatrix();
    }

    void Camera::processMouseMovement(
        float xOffset,
        float yOffset,
        bool constrainPitch)
    {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (constrainPitch)
        {
            pitch = std::clamp(pitch, -89.0f, 89.0f);
        }

        updateVectors();
        updateViewMatrix();
    }

    void Camera::processMouseScroll(float yOffset)
    {
        zoom -= yOffset;
        zoom = std::clamp(zoom, 1.0f, 90.0f);
    }

    void Camera::updateVectors()
    {
        const float yawRadians = toRadians(yaw);
        const float pitchRadians = toRadians(pitch);

        front[0] = std::cos(yawRadians) * std::cos(pitchRadians);
        front[1] = std::sin(pitchRadians);
        front[2] = std::sin(yawRadians) * std::cos(pitchRadians);

        normalize(front);

        cross(front, worldUp, right);
        normalize(right);

        cross(right, front, up);
        normalize(up);
    }

    void Camera::updateViewMatrix()
    {
        // Right vector.
        viewMatrix[0] = right[0];
        viewMatrix[1] = up[0];
        viewMatrix[2] = -front[0];
        viewMatrix[3] = 0.0f;

        viewMatrix[4] = right[1];
        viewMatrix[5] = up[1];
        viewMatrix[6] = -front[1];
        viewMatrix[7] = 0.0f;

        viewMatrix[8] = right[2];
        viewMatrix[9] = up[2];
        viewMatrix[10] = -front[2];
        viewMatrix[11] = 0.0f;

        viewMatrix[12] = -dot(right, position);
        viewMatrix[13] = -dot(up, position);
        viewMatrix[14] = dot(front, position);
        viewMatrix[15] = 1.0f;
    }

    const float* Camera::getViewMatrix() const
    {
        return viewMatrix;
    }

    const float* Camera::getPosition() const
    {
        return position;
    }

    const float* Camera::getFront() const
    {
        return front;
    }

    float Camera::getZoom() const
    {
        return zoom;
    }
}
