#include "Camera.h"
#include "Casts.h"
#include "Level.h"
const int windowHeight = 800;
const int cameraSpeed = 8;

Camera::Camera() : target(nullptr), playerTarget(nullptr) {
	view = sf::View();
    //draw(window);
}
void Camera::update(float deltaTime) {
    if (!target) { return; }
    sf::Vector2f start = view.getCenter();
    sf::Vector2f end = target->getPosition();
    if (playerTarget) {
        end = playerTarget->getCameraPosition(view);
        sf::Vector2f centerDistance = end - view.getCenter();
        float overshootX = abs(centerDistance.x) - 5.0f;
        float overshootY = abs(centerDistance.y) - 5.0f;
        if (overshootX > 0.0f) {
            if (centerDistance.x > 0.0f) {
                start.x += overshootX;
            }
            else {
                start.x -= overshootX;
            }
        }
        if (overshootY > 0.0f) {
            if (centerDistance.y > 0.0f) {
                start.y += overshootY;
            }
            else {
                start.y -= overshootY;
            }
        }
    }
    else {
        start = end + (start - end) * std::exp(-deltaTime * cameraSpeed);
    }
    sf::Vector2f viewTopLeft = start - view.getSize()/2.0f;
    sf::Vector2f viewBottomRight = start + view.getSize() / 2.0f;
    sf::Vector2f levelSize = Level::getBottomRight() - Level::getTopLeft();
    sf::Vector2f levelMiddle = Level::getTopLeft() + levelSize / 2.0f;
    if (levelSize.x > view.getSize().x) {
        float leftDistance = Level::getTopLeft().x - viewTopLeft.x;
        float rightDistance = Level::getBottomRight().x - viewBottomRight.x;
        if (leftDistance > 0) {
            start.x += leftDistance;
        }
        if (rightDistance < 0) {
            start.x += rightDistance;
        }
    }
    else {
        start.x = levelMiddle.x;
    }
    if (levelSize.y > view.getSize().y) {
        float topDistance = Level::getTopLeft().y - viewTopLeft.y;
        float bottomDistance = Level::getBottomRight().y - viewBottomRight.y;
        if (topDistance > 0) {
            start.y += topDistance;
        }
        if (bottomDistance < 0) {
            start.y += bottomDistance;
        }
    }
    else {
        start.y = levelMiddle.y;
    }
    view.setCenter(start);
}
void Camera::setTarget(sf::Transformable* newTarget) {
    playerTarget = nullptr;
    target = newTarget;
    view.setCenter(target->getPosition());
    update(0);
}
void Camera::setTarget(Player* newTarget) {
    setTarget(newTarget->transform);
    playerTarget = newTarget;
}
void Camera::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    if (windowSize.x < 1) {
        windowSize.x = 1;
    }
    if (windowSize.y < 1) {
        windowSize.y = 1;
    }
    sf::Vector2f size(
        windowHeight,
        windowHeight
       );
    if (windowSize.x > windowSize.y) {
        size.x *= windowSize.x * 1.0f / windowSize.y;
    }
    else {
        size.y *= windowSize.y * 1.0f / windowSize.x;
    }
    size = Casts::get().pixelsToMeters(size);
    view.setSize(size);
    window.setView(view);
}