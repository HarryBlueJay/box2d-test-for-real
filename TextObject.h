#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class TextObject : public Object {
public:
	//shapes//
	sf::Text text;
	TextObject(sf::Font& font): text(font) {}

	void draw(sf::RenderWindow& window) {
		window.draw(text);
	}
};