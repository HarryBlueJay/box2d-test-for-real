#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class Camera : public Object {
public:
	Camera();
	void update(sf::RenderWindow& window, float deltaTime) override;
	void inputCallback(sf::Event event) override;
	void setTarget(sf::RectangleShape* target);

private:
	sf::RectangleShape* target;
	sf::View view;
};