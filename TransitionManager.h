#pragma once
#include "BasicIncludes.h"
#include "Singleton.h"
class TransitionManager: public Singleton<TransitionManager> {
private:
	int transitionState = 0;
	const float fadeBoxSize = 100000;
	const float fadeTime = 1;
	float fadeTimeCounter = 0;
	int transitionLevel = 0;
	sf::RectangleShape fadeBox;
public:
	TransitionManager();
	void finishLevel(int nextLevel);
	void restartLevel();
	void draw(sf::RenderWindow& window) override;
	void update(float deltaTime) override;
	bool isTransitioning() const;
};