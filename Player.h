#pragma once

class Player
{
public:
	//hitboxes//
	// left hitbox
	// right hitbox
	// up hitbox
	// down hitbox
	bool bottomLine = false;
	bool topLine = false;
	bool leftLine = false;
	bool rightLine = false;

	//state//
	//Game::PlayerState state;
	bool canJump;

	//movement//
	int leftWallJumps = 0;
	int rightWallJumps = 0;

	//shapes//
	b2BodyId player;

	Player(b2BodyId playerId) {
		player = playerId;
	}

	void movePlayer() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			b2Body_ApplyForceToCenter(player, { -20,0 }, true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			b2Body_ApplyForceToCenter(player, { 20,0 }, true);
		}
	}

	void onGround(RenderWindow& window) {
		b2RayResult result = simpleRaycast(b2Body_GetWorldPoint(player, { 0,0 }), { 0,0.6 }, window);
		b2RayResult result2 = simpleRaycast(b2Body_GetWorldPoint(player, { -0.5,0 }), { 0,0.6 }, window);
		b2RayResult result3 = simpleRaycast(b2Body_GetWorldPoint(player, { 0.5,0 }), { 0,0.6 }, window);

		if (result.hit || result2.hit || result3.hit) {
			canJump = true;
		}
	}
	void movePlayerEvents(sf::Event event) {
		if (event.key.code == sf::Keyboard::Key::Space && canJump) {
			b2Body_ApplyLinearImpulseToCenter(player, { 0,-10 }, true);
			canJump = false;
		}
	}
};