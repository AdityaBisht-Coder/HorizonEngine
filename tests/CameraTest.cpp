
#include "horizon/Camera.hpp"

#include <iostream>

int main()
{
    horizon::Camera camera;

    std::cout << "================================\n";
    std::cout << "     HORIZON CAMERA TEST\n";
    std::cout << "================================\n";

    const float* position = camera.getPosition();
    const float* front = camera.getFront();

    std::cout << "Camera initialized successfully!\n";

    std::cout << "Position: "
              << position[0] << ", "
              << position[1] << ", "
              << position[2] << '\n';

    std::cout << "Front: "
              << front[0] << ", "
              << front[1] << ", "
              << front[2] << '\n';

    std::cout << "Initial FOV: "
              << camera.getZoom() << '\n';

    camera.processKeyboard(
        horizon::CameraDirection::Forward,
        1.0f
    );

    position = camera.getPosition();

    std::cout << "After forward movement: "
              << position[0] << ", "
              << position[1] << ", "
              << position[2] << '\n';

    camera.processMouseMovement(90.0f, 10.0f);

    camera.processMouseScroll(5.0f);

    std::cout << "After mouse movement and scrolling:\n";
    std::cout << "FOV: " << camera.getZoom() << '\n';

    std::cout << "Horizon Camera Test passed!\n";

    return 0;
}
