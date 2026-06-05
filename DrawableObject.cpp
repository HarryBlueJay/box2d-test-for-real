#include "DrawableObject.h"

void DrawableObject::draw(sf::RenderWindow& window) {
	sf::Vector2f position = rectangle.getPosition();
	sf::Vector2f offset = position - window.getView().getCenter();
	rectangle.setPosition(window.getView().getCenter() + offset * parallaxFactor);
	window.draw(rectangle);
	rectangle.setPosition(position);
}