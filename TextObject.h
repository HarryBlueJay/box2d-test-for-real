#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class TextObject : public DrawableObject {
private:
	//shapes//
	sf::Text text;
public:
	TextObject(sf::Texture* _texture, float _parallaxFactor, sf::Font& font):
		text(font),
		DrawableObject(&text, _texture, _parallaxFactor) {}
};