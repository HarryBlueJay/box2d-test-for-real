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
	b2SurfaceMaterial bodyIdMaterial = b2DefaultSurfaceMaterial();
	playerShapeDef.density /= 4;
	bodyIdMaterial.friction = 0.5f;
	playerShapeDef.material = bodyIdMaterial;
	sf::Vector2f size = sf::Vector2f(64, 64);
	Casts::makeCircleWithBodyDef(*getConvexShape(), bodyId, playerShapeDef, Casts::b2Vec2_to_sfVector2f(spawnLocation), size, 0, bodyDef);

	bodyTexture.loadFromFile("resources/body.png");
	getShape()->setTexture(&bodyTexture);
	eyeTexture.loadFromFile("resources/eye.png");
	eye.setTexture(&eyeTexture);
	eye.setSize(size/32.0f);
	eye.setOrigin(transform->getOrigin());
	b2Body_SetLinearVelocity(bodyId, { 0,0 });
	//b2Body_SetTransform(bodyId, spawnLocation, b2Body_GetRotation(bodyId));
	Casts::move(*transform, bodyId);
	dying = false;
	eye.setFillColor(sf::Color::Black);
}
void Player::die() {
	b2Body_SetLinearVelocity(bodyId, { 0,0 });
	dying = true;
}

sf::Vector2f Player::getCameraPosition(sf::View& view) {
	sf::Vector2f position = Casts::b2Vec2_to_sfVector2f(b2Body_GetPosition(bodyId));
	position.y -= view.getSize().y / 8;
	return position;
}

void Player::update(float deltaTime) {
	coyoteCounter -= deltaTime;
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
	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
	
	sf::Vector2f end = Casts::b2Vec2_to_sfVector2f(b2Normalize(linearVelocity)) / 2.5f;
	eyeOffset = end + (eyeOffset - end) * std::exp(-deltaTime * 10);
	
	//b2Body_SetLinearVelocity(bodyId, { std::clamp(linearVelocity.x,-maxWalkingSpeed,maxWalkingSpeed), linearVelocity.y });
	float xForce = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		xForce -= walkForce;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		xForce += walkForce;
	}
	b2ShapeId shape;
	b2Body_GetShapes(bodyId, &shape, 1);
	//if (xForce != 0) {
	//	b2Shape_SetFriction(shape, 0.0f);
	//}
	//else {
	//	b2Shape_SetFriction(shape, 1.f);
	//}
	if (abs(linearVelocity.x) < 15 || std::signbit(xForce) != std::signbit(linearVelocity.x)) {
		b2Body_ApplyForceToCenter(bodyId, { xForce, 0 }, true);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && coyoteCounter > 0) {
		if (!touchingLeft && !touchingRight) {
			b2Body_SetLinearVelocity(bodyId, b2Vec2{ linearVelocity.x, -jumpSpeed });
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
			std::cout << touchingWall << std::endl;
			if (std::signbit(wallJumps) == std::signbit(touchingWall)) {
				wallJumps += touchingWall;
			}
			else {
				wallJumps = touchingWall;
			}
			b2Body_SetLinearVelocity(bodyId, b2Normalize({ touchingWall * -1.0f, -2.0f + abs(wallJumps / 4.0f) }) * jumpSpeed);
		}
		coyoteCounter = 0.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::End)) {
		b2Body_ApplyForceToCenter(bodyId, { 0,-100 }, true);
	}
	if (b2Body_GetPosition(bodyId).y > Casts::pixelsToMeters(1080)) {
		die();
	}
	touchingLeft = false;
	touchingRight = false;
	touchingFloor = false;
	Casts::move(*transform, bodyId);
}
void Player::collide(Object* otherObject, b2Vec2 normal) {
	LevelPolygon* levelRectangle = dynamic_cast<LevelPolygon*>(otherObject);
	if (levelRectangle) {
		if (levelRectangle->nextLevel >= 0) {
			TransitionManager::get().finishLevel(levelRectangle->nextLevel);
		}
		if (levelRectangle->isKillbrick) {
			die();
		}
		if (levelRectangle->gravityStrength >= 0) {
			b2Vec2 newGravity = Casts::sfVector2f_to_b2Vec2(levelRectangle->transform->getTransform() * sf::Vector2f(0, 1)) * levelRectangle->gravityStrength;
			b2World_SetGravity(b2Body_GetWorld(bodyId), newGravity);
		}
	}

	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
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
				direction = b2Body_GetLinearVelocity(bodyId);
			}
			direction = b2Normalize(direction) * dashSpeed;
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

	window.draw(&path[0], path.size(), sf::PrimitiveType::LineStrip);
}