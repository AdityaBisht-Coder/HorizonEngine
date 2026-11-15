
#pragma once

namespace horizon
{
    enum class CameraDirection
    {
        Forward,
        Backward,
        Left,
        Right
    };

    class Camera
    {
    public:
        Camera();

        void processKeyboard(
            CameraDirection direction,
            float deltaTime
        );

        void processMouseMovement(
            float xOffset,
            float yOffset,
            bool constrainPitch = true
        );

        void processMouseScroll(float yOffset);

        const float* getViewMatrix() const;
        const float* getPosition() const;
        const float* getFront() const;

        float getZoom() const;

    private:
        void updateVectors();
        void updateViewMatrix();

        float position[3];
        float front[3];
        float up[3];
        float right[3];
        float worldUp[3];

        float yaw;
        float pitch;

        float movementSpeed;
        float mouseSensitivity;
        float zoom;

        float viewMatrix[16];
    };
}
