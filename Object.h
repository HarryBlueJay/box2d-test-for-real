#pragma once

class Object
{
public:
	virtual void update(sf::RenderWindow& window, float deltaTime) {}

	virtual void inputCallback(sf::Event event) {}

	virtual void draw(sf::RenderWindow& window) {}
};