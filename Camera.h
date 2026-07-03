#pragma once
#include "BasicIncludes.h"
#include "Player.h"
#include "Object.h"

class Camera : public Object {
public:
	Camera(sf::RenderWindow& window);
	void update(float deltaTime) override;
	//void inputCallback(std::optional<sf::Event> event) override;
	void setTarget(sf::Transformable* target);
	void setTarget(Player* playerTarget);
	void setBounds(sf::Vector2f boundTopLeft, sf::Vector2f boundBottomRight);
	void draw(sf::RenderWindow& window) override;

private:
	sf::Transformable* target;
	Player* playerTarget;
	sf::View view;
	sf::Vector2f topLeft;
	sf::Vector2f bottomRight;
};