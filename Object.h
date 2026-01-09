#pragma once

class Object
{
public:
	virtual void update(float deltaTime) {}

	virtual void inputCallback(sf::Event event) {}

	virtual void draw(sf::RenderWindow& window) {}
};