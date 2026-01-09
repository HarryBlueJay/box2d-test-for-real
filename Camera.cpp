#include "Camera.h"
#include "Casts.h"
const int windowHeight = 800;

Camera::Camera(sf::RenderWindow& window) : target(nullptr), playerTarget(nullptr) {
	view = sf::View();
    draw(window);
}
void Camera::update(float deltaTime) {
    if (!target) { return; }
    sf::Vector2f start = view.getCenter();
    sf::Vector2f end = target->getPosition();
    if (playerTarget) {
        end = playerTarget->getCameraPosition(view);
    }
    start = end + (start - end) * std::exp(-deltaTime * 8);
    sf::Vector2f viewTopLeft = start - view.getSize()/2.0f;
    sf::Vector2f viewBottomRight = start + view.getSize() / 2.0f;
    float leftDistance = topLeft.x - viewTopLeft.x;
    float topDistance = topLeft.y - viewTopLeft.y;
    float rightDistance = bottomRight.x - viewBottomRight.x;
    float bottomDistance = bottomRight.y - viewBottomRight.y;
    if (leftDistance > 0) {
        start.x += leftDistance;
    }
    if (topDistance > 0) {
        start.y += topDistance;
    }
    if (rightDistance < 0) {
        start.x += rightDistance;
    }
    if (bottomDistance < 0) {
        start.y += bottomDistance;
    }
    view.setCenter(start);
}
void Camera::setTarget(sf::RectangleShape* newTarget) {
    playerTarget = nullptr;
    target = newTarget;
    view.setCenter(target->getPosition());
    update(0);
}
void Camera::setTarget(Player* newTarget) {
    setTarget(&newTarget->rectangle);
    playerTarget = newTarget;
}
void Camera::setBounds(sf::Vector2f boundTopLeft, sf::Vector2f boundBottomRight) {
    topLeft = boundTopLeft;
    bottomRight = boundBottomRight;
}
void Camera::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f size(
        Casts::pixelsToMeters(windowHeight * windowSize.x / windowSize.y),
        Casts::pixelsToMeters(windowHeight)
       );
    view.setSize(size);
    window.setView(view);
}