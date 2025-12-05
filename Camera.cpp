#include "Camera.h"
#include "Casts.h"
const int windowHeight = 800;

Camera::Camera(): target(nullptr), playerTarget(nullptr) {
	view = sf::View();
}
void Camera::update(sf::RenderWindow& window, float deltaTime) {
    if (!target) { return; }
    if (playerTarget && !playerTarget->getCanJump()) { return; }
    sf::Vector2f start = view.getCenter();
    sf::Vector2f end = target->getPosition();
    start = end + (start - end) * std::exp(-deltaTime * 15);

    view.setCenter(start);
}
void Camera::inputCallback(sf::Event event) {
    if (event.type == sf::Event::Resized) {
        view.setSize({
                static_cast<float>(Casts::pixelsToMeters(event.size.width)),
                static_cast<float>(Casts::pixelsToMeters(event.size.height))
            });
        // source: https://stackoverflow.com/questions/61447069/sfml-window-resizing-is-very-ugly
    }
}
void Camera::setTarget(sf::RectangleShape* newTarget) {
    playerTarget = nullptr;
    target = newTarget;
    view.setCenter(target->getPosition());
}
void Camera::setTarget(Player* newTarget) {
    setTarget(&newTarget->rectangle);
    playerTarget = newTarget;
}
void Camera::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    view.setSize({
                static_cast<float>(Casts::pixelsToMeters(windowHeight * windowSize.x / windowSize.y)),
                static_cast<float>(Casts::pixelsToMeters(windowHeight))
        });
    window.setView(view);
}