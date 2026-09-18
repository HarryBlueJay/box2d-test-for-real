#include "DrawableObject.h"

DrawableObject::DrawableObject(sf::Transformable* _transform, float _parallaxFactor) :
	transform(_transform),
	parallaxFactor(_parallaxFactor) {}
DrawableObject::~DrawableObject() {
	if (texture) {
		delete texture;
		texture = nullptr;
	}
	if (transform) {
		delete transform;
		transform = nullptr;
	}
}
void DrawableObject::draw(sf::RenderWindow& window) {
	sf::Vector2f center = window.getView().getCenter();
	sf::Vector2f position = transform->getPosition();
	transform->setPosition(position - center + center / parallaxFactor);
	window.draw(*dynamic_cast<sf::Drawable*>(transform));
	transform->setPosition(position);
}
void DrawableObject::addTexture(const std::string& texturePath) {
	texture = new sf::Texture(texturePath);
	texture->setSmooth(false);
	getConvexShape()->setTexture(texture);
}