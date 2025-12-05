#pragma once
#include "BasicIncludes.h"
#include "Player.h"
#include "Object.h"

class Camera : public Object {
public:
	Camera();
	void update(sf::RenderWindow& window, float deltaTime) override;
	void inputCallback(sf::Event event) override;
	void setTarget(sf::RectangleShape* target);
	void setTarget(Player* playerTarget);
	void draw(sf::RenderWindow& window) override;

private:
	sf::RectangleShape* target;
	Player* playerTarget;
	sf::View view;
};