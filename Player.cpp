#include "Player.h"
#include "Level.h"
#include "Casts.h"
#include "TransitionManager.h"
sf::Texture bodyTexture;
sf::Texture eyeTexture;


Player::Player(b2Vec2 spawnLocation) {
	transform = new sf::ConvexShape();
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;

	b2ShapeDef playerShapeDef = b2DefaultShapeDef();
	playerShapeDef.filter.maskBits = PLAYER | LEVEL;
	playerShapeDef.filter.categoryBits = PLAYER;
	playerShapeDef.enableSensorEvents = true;
	playerShapeDef.enableContactEvents = true;
	b2SurfaceMaterial bodyIdMaterial = b2DefaultSurfaceMaterial();
	playerShapeDef.density /= 4;
	bodyIdMaterial.friction = 0.5f;
	playerShapeDef.material = bodyIdMaterial;
	sf::Vector2f playerSize = Casts::get().b2Vec2_to_sfVector2f(size);
	Casts::get().makeCircleWithBodyDef(*getConvexShape(), bodyId, playerShapeDef, Casts::get().b2Vec2_to_sfVector2f(spawnLocation), playerSize / Casts::get().scaleFactor, 0, bodyDef);

	auto _ = bodyTexture.loadFromFile("resources/body.png");
	getShape()->setTexture(&bodyTexture);
	_ = eyeTexture.loadFromFile("resources/eye.png");
	eye.setTexture(&eyeTexture);
	eye.setSize(playerSize);
	eye.setOrigin(transform->getOrigin());
	b2Body_SetLinearVelocity(bodyId, { 0,0 });
	//b2Body_SetTransform(bodyId, spawnLocation, b2Body_GetRotation(bodyId));
	Casts::get().move(*transform, bodyId);
	dying = false;
	eye.setFillColor(sf::Color::Black);

	gravityPath[0].color = sf::Color::Red;
	gravityPath[1].color = sf::Color::Red;
}
void Player::die() {
	b2Body_SetLinearVelocity(bodyId, { 0,0 });
	dying = true;
}

sf::Vector2f Player::getCameraPosition(sf::View& view) {
	sf::Vector2f position = Casts::get().b2Vec2_to_sfVector2f(b2Body_GetPosition(bodyId));
	position.y -= view.getSize().y / 8;
	return position;
}

void Player::update(float deltaTime) {
	const float delta = 0.05f;
	const float deathRotationSpeed = 360;
	const float deathAnimationTime = 0.75f;
	if (dying) {
		deltaTime /= deathAnimationTime;
		sf::Vector2f shrink = sf::Vector2f(deltaTime, deltaTime);
		sf::Vector2f center = getShape()->getGlobalBounds().getCenter();
		transform->setScale(transform->getScale() - shrink);
		transform->move(shrink / 2.0f);
		transform->rotate(sf::degrees(deltaTime * deathRotationSpeed));
		sf::Vector2f newCenter = getShape()->getGlobalBounds().getCenter();
		sf::Vector2f centerOffset = center - newCenter;
		transform->move(centerOffset);
		eye.setPosition(transform->getPosition());
		eye.setRotation(transform->getRotation());
		eye.setScale(transform->getScale());
		if (transform->getScale().x < delta) {
			TransitionManager::get().restartLevel();
		}
		return;
	}
	bool touchingFloor = Casts::get().circlecast(b2Body_GetWorldCenterOfMass(bodyId), size.x / 2.0f, Level::rotateByGravity(b2Vec2{ 0, 100 })).hit;
	bool touchingLeft = Casts::get().circlecast(b2Body_GetWorldCenterOfMass(bodyId), size.x / 2.0f, Level::rotateByGravity(b2Vec2{ -1, 0 })).hit;
	bool touchingRight = Casts::get().circlecast(b2Body_GetWorldCenterOfMass(bodyId), size.x / 2.0f, Level::rotateByGravity(b2Vec2{ 1, 0 })).hit;
	if (!touchingFloor) {
		coyoteCounter -= deltaTime;
	}
	else {
		coyoteCounter = coyoteTime;
		wallJumps = 0;
		lastGroundedPosition = transform->getPosition();
		dashes = maxDashes;
		eye.setFillColor(sf::Color::Black);
	}
	if (touchingLeft || touchingRight) {
		coyoteCounter = coyoteTime;
	}
	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
	
	sf::Vector2f end = Casts::get().b2Vec2_to_sfVector2f(b2Normalize(linearVelocity)) / 2.5f;
	eyeOffset = end + (eyeOffset - end) * std::exp(-deltaTime * 10);

	linearVelocity = Level::unrotateByGravity(linearVelocity);
	
	float xForce = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		xForce -= walkForce;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		xForce += walkForce;
	}
	if (abs(linearVelocity.x) < 15 || std::signbit(xForce) != std::signbit(linearVelocity.x)) {
		b2Body_ApplyForceToCenter(bodyId, Level::rotateByGravity({ xForce, 0 }), true);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && coyoteCounter > 0) {
		if (!touchingLeft && !touchingRight) {
			b2Body_SetLinearVelocity(bodyId, Level::rotateByGravity(b2Vec2{ linearVelocity.x, -jumpSpeed }));
		}
		else {
			float touchingWall = 0;
			if (touchingLeft) {
				touchingWall += 1;
			}
			if (touchingRight) {
				touchingWall -= 1;
			}
			if (touchingFloor) {
				touchingWall /= 10;
			}
			if (std::signbit(wallJumps) == std::signbit(touchingWall)) {
				wallJumps += static_cast<int>(touchingWall);
			}
			else {
				wallJumps = static_cast<int>(touchingWall);
			}
			b2Body_SetLinearVelocity(bodyId, Level::rotateByGravity(b2Normalize({ touchingWall * -1.0f, -2.0f + abs(wallJumps / 4.0f) }) * jumpSpeed));
		}
		coyoteCounter = 0.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::End)) {
		b2Body_ApplyForceToCenter(bodyId, Level::rotateByGravity({ 0,-100 }), true);
	}
	b2Vec2 position = b2Body_GetPosition(bodyId);
	sf::Vector2f topLeft = Level::getTopLeft();
	sf::Vector2f bottomRight = Level::getBottomRight();
	if (position.y < topLeft.y || position.y > bottomRight.y || position.x < topLeft.x || position.x > bottomRight.x ) {
		die();
	}

	Casts::get().move(*transform, bodyId);
}
void Player::collide(Object* otherObject, b2Vec2 normal) {
	touch(otherObject);
	/*b2Vec2 linearVelocity = Level::unrotateByGravity(b2Body_GetLinearVelocity(bodyId));
	normal = Level::unrotateByGravity(normal);
	if (normal.y < -0.5f) {
		coyoteCounter = coyoteTime;
		touchingFloor = true;
		wallJumps = 0;
		lastGroundedPosition = transform->getPosition();
		dashes = maxDashes;
		eye.setFillColor(sf::Color::Black);
	}
	if (linearVelocity.y >= -20.0f) {
		if (normal.x > 0.9) {
			coyoteCounter = coyoteTime;
			touchingRight = true;
		}
		else if (normal.x < -0.9) {
			coyoteCounter = coyoteTime;
			touchingLeft = true;
		}
	}*/
}
void Player::touch(Object* otherObject) {
	LevelPolygon* levelRectangle = dynamic_cast<LevelPolygon*>(otherObject);
	if (levelRectangle) {
		if (levelRectangle->nextLevel >= 0) {
			TransitionManager::get().finishLevel(levelRectangle->nextLevel);
		}
		if (levelRectangle->isKillbrick) {
			die();
		}
		if (levelRectangle->gravityStrength >= 0) {
			b2Vec2 newGravity = Casts::get().sfVector2f_to_b2Vec2(sf::Vector2f(0, 1).rotatedBy(levelRectangle->transform->getRotation())) * levelRectangle->gravityStrength;
			b2World_SetGravity(b2Body_GetWorld(bodyId), newGravity);
			//b2Transform newTransform = b2Body_GetTransform(bodyId);
			//newTransform.q = b2MakeRot(levelRectangle->transform->getRotation().asRadians());
			//b2Body_SetTargetTransform(bodyId, newTransform, 1);
		}
	}
}

void Player::inputCallback(std::optional<sf::Event> event) {
	if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::Delete) {
			die();
		}
		if (keyPressed->code == sf::Keyboard::Key::Space && dashes != 0 && coyoteCounter <= 0.0f) {
			b2Vec2 direction = {};
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
				direction.x -= 1;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
				direction.x += 1;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
				direction.y -= 1;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
				direction.y += 1;
			}
			if (direction.x == 0 && direction.y == 0) {
				direction = Level::unrotateByGravity(b2Body_GetLinearVelocity(bodyId));
			}
			direction = Level::rotateByGravity(b2Normalize(direction) * dashSpeed);
			b2Body_SetLinearVelocity(bodyId, direction);
			dashes--;
			eye.setFillColor(sf::Color(200, 200, 200));
		}
	}
}

void Player::draw(sf::RenderWindow& window) {
	sf::Vector2f rotatedEyeOffset = eyeOffset.rotatedBy(-transform->getRotation());
	eye.setPosition(transform->getPosition() + (rotatedEyeOffset * eye.getScale().x));
	window.draw(*getShape());
	window.draw(eye);
	path.push_back({ transform->getPosition(), sf::Color::Green });
	if (path.size() > 14400) {
		path.erase(path.begin());
	}
	return;
	gravityPath[0].position = transform->getPosition() + sf::Vector2f(1, 1);
	gravityPath[1].position = gravityPath[0].position + Casts::get().scaleFactor * Casts::get().b2Vec2_to_sfVector2f(b2World_GetGravity(b2Body_GetWorld(bodyId)));

	window.draw(&path[0], path.size(), sf::PrimitiveType::LineStrip);
	window.draw(gravityPath, 2, sf::PrimitiveType::LineStrip);
}