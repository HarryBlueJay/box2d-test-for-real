#include "TransitionManager.h"
#include "Level.h"

TransitionManager::TransitionManager() {
	fadeBox = sf::RectangleShape(sf::Vector2f(fadeBoxSize, fadeBoxSize));
	fadeBox.setOrigin(fadeBoxSize/2, fadeBoxSize/2);
	fadeBox.setFillColor(sf::Color::Transparent);
}
bool TransitionManager::isTransitioning() {
	return transitionState != 0;
}
void TransitionManager::finishLevel() {
	transitionState = 1;
	fadeTimeCounter = 0;
	transitionLevel = Level::getCurrentLevel() + 1;
}
void TransitionManager::restartLevel() {
	transitionState = 1;
	fadeTimeCounter = 0;
	transitionLevel = Level::getCurrentLevel();
}

void TransitionManager::draw(sf::RenderWindow& window) {
	window.draw(fadeBox);
}

void TransitionManager::update(float deltaTime) {
	fadeTimeCounter = std::clamp(fadeTimeCounter + (deltaTime * transitionState), 0.0f, fadeTime);
	if (fadeTimeCounter == fadeTime) {
		Level::loadLevel(transitionLevel);
		transitionState = -1;
	}
	if (fadeTimeCounter == 0) {
		transitionState = 0;
	}
	fadeBox.setFillColor(sf::Color(0, 0, 0, (fadeTimeCounter / fadeTime) * 255));
}
