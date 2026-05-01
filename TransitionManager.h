#pragma once
#include "BasicIncludes.h"
#include "Object.h"
class TransitionManager: public Object {
private:
	int transitionState = 0;
	const float fadeBoxSize = 100000;
	const float fadeTime = 1;
	float fadeTimeCounter = 0;
	int transitionLevel = 0;
	sf::RectangleShape fadeBox;
	TransitionManager();
public:
	static TransitionManager& get() {
		static TransitionManager singleton;
		return singleton;
	};
	void finishLevel(int nextLevel);
	void restartLevel();
	void draw(sf::RenderWindow& window) override;
	void update(float deltaTime) override;
	bool isTransitioning();
};