#include <iostream>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Casts.h"

using namespace sf;
const int WINDOWWIDTH = 800;
const int WINDOWHEIGHT = 600;
float scaleFactor = 1.0f / 32.0f; // multiple of 2 to avoid precision issues
//also anything below 4 pixels causes trouble, don't expect to reach that

//temporary variables (no i will not make a namespace)
bool playerCanJump = false;

b2WorldDef worldDef;
b2WorldId worldId;


float pixelsToMeters(float input) {
    return input * scaleFactor;
}
void move(sf::RectangleShape& rectangle, b2BodyId& id) {
    //b2Body_GetPosition()
    //b2Body_GetRotation()
    rectangle.setPosition(b2Vec2_to_sfVector2f(b2Body_GetPosition(id)));
    //std::cout << b2Rot_GetAngle(b2Body_GetRotation(id)) * 180/B2_PI << std::endl;
    rectangle.setRotation(b2Rot_GetAngle(b2Body_GetRotation(id)) * 180 / B2_PI);
}
void makeBoxWithBodyDef(sf::RectangleShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, b2BodyDef bodyDef) {
    position = sf::Vector2f(pixelsToMeters(position.x), pixelsToMeters(position.y));
    size = sf::Vector2f(pixelsToMeters(size.x), pixelsToMeters(size.y));
    bodyDef.position = sfVector2f_to_b2Vec2(position);
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(size.x / 2, size.y / 2); // eventually do stuff with the scale factor
    id = b2CreateBody(worldId, &bodyDef);
    b2CreatePolygonShape(id, &shapeDef, &dynamicBox);

    box.setSize(size);
    box.setOrigin(size.x / 2, size.y / 2);
    move(box, id);
}
// maybe make a version that takes in a body def
void makeBox(sf::RectangleShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, b2BodyType bodyType) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = bodyType;
    bodyDef.position = sfVector2f_to_b2Vec2(position);
    makeBoxWithBodyDef(box, id, shapeDef,position,size,bodyDef);
} 
void movePlayer(b2BodyId id) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        b2Body_ApplyForceToCenter(id, { -20,0 }, true);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        b2Body_ApplyForceToCenter(id, { 20,0 }, true);
    }
}

void onGroundHopefullyWillWork(b2BodyId id, RenderWindow& window){
    b2RayResult result = simpleRaycast(b2Body_GetWorldPoint(id, { 0,0 }), {0,0.6}, window);
    b2RayResult result2 = simpleRaycast(b2Body_GetWorldPoint(id, { -0.5,0 }), {0,0.6}, window);
    b2RayResult result3 = simpleRaycast(b2Body_GetWorldPoint(id, { 0.5,0 }), {0,0.6}, window);

    if (result.hit || result2.hit || result3.hit) {
        playerCanJump = true;
    }
}
void movePlayerEvents(b2BodyId id, sf::Event event) {
    if (event.key.code == sf::Keyboard::Key::Space && playerCanJump) {
        b2Body_ApplyLinearImpulseToCenter(id, { 0,-10 }, true);
        playerCanJump = false;
    }
}

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 8.25f };
    worldId = b2CreateWorld(&worldDef);
    castWorldId = worldId;
    // Making moving box
    sf::RectangleShape testShape{};
    b2BodyId bodyId{};
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 1.f;
    makeBox(testShape, bodyId, shapeDef, sf::Vector2f(50, 4.0f), sf::Vector2f(4.0f, 4.0f), b2_dynamicBody);

    sf::RectangleShape playerBox{};
    b2BodyId playerBoxId{};
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    makeBoxWithBodyDef(playerBox, playerBoxId, b2DefaultShapeDef(), sf::Vector2f(50, 100), sf::Vector2f(32, 32), bodyDef);

    // Making floor
    sf::RectangleShape floor{};
    b2BodyId groundId{};
    makeBox(floor, groundId, b2DefaultShapeDef(), sf::Vector2f(50, 500), sf::Vector2f(50, 10), b2_staticBody);

    //b2Body_SetAngularVelocity(playerBoxId, 100000);

    RenderWindow window(VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Hello Physics");
    sf::View view(sf::FloatRect({ 0,0 }, { pixelsToMeters(WINDOWWIDTH) , pixelsToMeters(WINDOWHEIGHT) }));

    Clock clock;
    Time lastTime = clock.getElapsedTime();
    window.setFramerateLimit(165);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                movePlayerEvents(playerBoxId, event);
            }
        }
        Time currentTime = clock.getElapsedTime();

        movePlayer(playerBoxId);
        int subStepCount = 4;
        b2World_Step(worldId, 1. / 165, subStepCount);
        move(testShape, bodyId);
        move(playerBox, playerBoxId);

        window.setView(view);
        window.clear();
        window.draw(testShape);
        window.draw(floor);
        window.draw(playerBox);
        //onGround(playerBoxId, window);
        //onGroundBroken(playerBoxId,window);
        playerCanJump = false;
        onGroundHopefullyWillWork(playerBoxId, window);
        //simpleLinecast(b2Body_GetWorldPoint(playerBoxId, { -0.5,-0.5 }), b2Body_GetWorldPoint(playerBoxId, { -0.5,0.5 }), { -0.25,0 }, window);
        OverlapResult result = lineOverlap(b2Body_GetWorldPoint(playerBoxId, { -0.6,-0.4 }), b2Body_GetWorldPoint(playerBoxId, { -0.6,0.4 }), b2DefaultQueryFilter(), window);
        window.display();
    }
    return 0;
}