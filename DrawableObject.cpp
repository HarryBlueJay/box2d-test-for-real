#include "DrawableObject.h"

void DrawableObject::draw(sf::RenderWindow& window) {
	sf::Vector2f center = window.getView().getCenter();
	sf::Vector2f position = rectangle.getPosition();
	rectangle.setPosition(position - center + center / parallaxFactor);
	window.draw(rectangle);
	rectangle.setPosition(position);
}