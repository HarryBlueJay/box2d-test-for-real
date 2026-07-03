#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class TextObject : public DrawableObject {
public:
	//shapes//
	sf::Text text;
	TextObject(sf::Font& font): text(font) {
		transform = &text;
	}
};