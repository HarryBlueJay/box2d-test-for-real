#include "Camera.h"
#include "Casts.h"

Camera::Camera() {
	view = sf::View();
}
void Camera::update(sf::RenderWindow& window, float deltaTime) {
    if (!target) { return; }
    sf::Vector2f start = view.getCenter();
    sf::Vector2f end = target->getPosition();
    start = end + (start - end) * std::exp(-deltaTime * 15);

    view.setCenter(start);
    sf::Vector2u windowSize = window.getSize();
    view.setSize({
                static_cast<float>(Casts::pixelsToMeters(windowSize.x)),
                static_cast<float>(Casts::pixelsToMeters(windowSize.y))
        });
    window.setView(view);
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
    target = newTarget;
    view.setCenter(target->getPosition());
}