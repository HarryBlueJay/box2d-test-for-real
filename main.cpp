#include <iostream>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

using namespace sf;
const int WINDOWWIDTH = 800;
const int WINDOWHEIGHT = 600;
float scaleFactor = 1.0f/32.0f; // multiple of 2 to avoid precision issues
//also anything below 4 pixels causes trouble, don't expect to reach that

b2WorldDef worldDef;
b2WorldId worldId;

//vec2forSFML
sf::Vector2f b2Vec2_to_sfVector2f(b2Vec2 input) {
    return sf::Vector2f(input.x, input.y);
}
b2Vec2 sfVector2f_to_b2Vec2(sf::Vector2f input) {
    return b2Vec2{input.x, input.y};
}
void move(sf::RectangleShape& rectangle, b2BodyId& id) {
    //b2Body_GetPosition()
    //b2Body_GetRotation()
    rectangle.setPosition(b2Vec2_to_sfVector2f(b2Body_GetPosition(id)));
    //std::cout << b2Rot_GetAngle(b2Body_GetRotation(id)) * 180/B2_PI << std::endl;
    rectangle.setRotation(b2Rot_GetAngle(b2Body_GetRotation(id)) * 180/B2_PI);
}
float pixelsToMeters(float input) {
    return input * scaleFactor;
}
// maybe make a version that takes in a body def
void makeBox(sf::RectangleShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, b2BodyType bodyType) {
    position = sf::Vector2f(pixelsToMeters(position.x), pixelsToMeters(position.y));
    size = sf::Vector2f(pixelsToMeters(size.x), pixelsToMeters(size.y));
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = bodyType;
    bodyDef.position = sfVector2f_to_b2Vec2(position);
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(size.x/2, size.y/2); // eventually do stuff with the scale factor
    id = b2CreateBody(worldId, &bodyDef);
    b2CreatePolygonShape(id, &shapeDef, &dynamicBox);

    box.setSize(size);
    box.setOrigin(size.x / 2, size.y / 2);
    move(box, id);  
}

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 10.0f };
    worldId = b2CreateWorld(&worldDef);
    // Making moving box
    sf::RectangleShape testShape{};
    b2BodyId bodyId{};
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 1.f;
    makeBox(testShape, bodyId, shapeDef, sf::Vector2f(50, 4.0f), sf::Vector2f(2.5f, 2.5f), b2_dynamicBody);

    sf::RectangleShape movingBox{};
    b2BodyId movingBoxId{};
    makeBox(movingBox, movingBoxId, b2DefaultShapeDef(), sf::Vector2f(50, 100), sf::Vector2f(50, 50), b2_dynamicBody);

    // Making floor
    sf::RectangleShape floor{};
    b2BodyId groundId{};
    makeBox(floor, groundId, b2DefaultShapeDef(), sf::Vector2f(50, 500), sf::Vector2f(50, 10), b2_staticBody);

    b2Body_SetAngularVelocity(bodyId, 100);

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
        }
        Time currentTime = clock.getElapsedTime();

        int subStepCount = 4;
        b2World_Step(worldId, 1./165, subStepCount);
        move(testShape, bodyId);
        move(movingBox, movingBoxId);

        window.setView(view);
        window.clear();
        window.draw(testShape);
        window.draw(floor);
        window.draw(movingBox);
        window.display();
    }
    return 0;
}