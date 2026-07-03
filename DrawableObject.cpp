#include "DrawableObject.h"

void DrawableObject::draw(sf::RenderWindow& window) {
	sf::Vector2f center = window.getView().getCenter();
	sf::Vector2f position = transform->getPosition();
	transform->setPosition(position - center + center / parallaxFactor);
	window.draw(*dynamic_cast<sf::Drawable*>(transform));
	transform->setPosition(position);
}