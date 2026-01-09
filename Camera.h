#pragma once
#include "BasicIncludes.h"
#include "Player.h"
#include "Object.h"

class Camera : public Object {
public:
	Camera(sf::RenderWindow& window);
	void update(float deltaTime) override;
	//void inputCallback(sf::Event event) override;
	void setTarget(sf::RectangleShape* target);
	void setTarget(Player* playerTarget);
	void setBounds(sf::Vector2f boundTopLeft, sf::Vector2f boundBottomRight);
	void draw(sf::RenderWindow& window) override;

private:
	sf::RectangleShape* target;
	Player* playerTarget;
	sf::View view;
	sf::Vector2f topLeft;
	sf::Vector2f bottomRight;
};