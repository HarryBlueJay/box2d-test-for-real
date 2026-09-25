#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class TextObject : public DrawableObject {
public:
	//shapes//
	sf::Text text;

	TextObject(float _parallaxFactor, sf::Font& font):
		text(font),
		DrawableObject(&text, _parallaxFactor) {}
};