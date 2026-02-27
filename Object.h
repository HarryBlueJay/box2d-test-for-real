#pragma once

class Object
{
public:
	virtual void update(float deltaTime) {}

	virtual void inputCallback(std::optional<sf::Event> event) {}

	virtual void draw(sf::RenderWindow& window) {}
};