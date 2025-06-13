#include <iostream>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

using namespace sf;
const int WINDOWWIDTH = 800;
const int WINDOWHEIGHT = 600;
float scaleFactor = 1.0f / 32.0f; // multiple of 2 to avoid precision issues
//also anything below 4 pixels causes trouble, don't expect to reach that

b2WorldDef worldDef;
b2WorldId worldId;

//vec2forSFML
sf::Vector2f b2Vec2_to_sfVector2f(b2Vec2 input) {
    return sf::Vector2f(input.x, input.y);
}
b2Vec2 sfVector2f_to_b2Vec2(sf::Vector2f input) {
    return b2Vec2{ input.x, input.y };
}
void move(sf::RectangleShape& rectangle, b2BodyId& id) {
    //b2Body_GetPosition()
    //b2Body_GetRotation()
    rectangle.setPosition(b2Vec2_to_sfVector2f(b2Body_GetPosition(id)));
    //std::cout << b2Rot_GetAngle(b2Body_GetRotation(id)) * 180/B2_PI << std::endl;
    rectangle.setRotation(b2Rot_GetAngle(b2Body_GetRotation(id)) * 180 / B2_PI);
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
    b2Polygon dynamicBox = b2MakeBox(size.x / 2, size.y / 2); // eventually do stuff with the scale factor
    id = b2CreateBody(worldId, &bodyDef);
    b2CreatePolygonShape(id, &shapeDef, &dynamicBox);

    box.setSize(size);
    box.setOrigin(size.x / 2, size.y / 2);
    move(box, id);
}
void movePlayer(b2BodyId id) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        b2Body_ApplyForceToCenter(id, { -20,0 }, true);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        b2Body_ApplyForceToCenter(id, { 20,0 }, true);
    }
}


struct CastResult
{
    b2Vec2 point;
    b2BodyId bodyId;
    float fraction;
    bool hit;
};
static float CastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
    CastResult* result = (CastResult*)context;
    result->point = point;
    std::cout << normal.y << std::endl;
    result->bodyId = b2Shape_GetBody(shapeId);
    result->fraction = fraction;
    result->hit = true;
    return fraction;
}


void onGroundBroken(b2BodyId id, RenderWindow& window) {
    b2ShapeCastInput input{};
    CastResult context = {};
    b2ShapeId shapearray[1];
    b2Body_GetShapes(id, shapearray, 1);
    b2Polygon polygon = b2Shape_GetPolygon(shapearray[0]);
    //b2Polygon polygon = b2MakeOffsetBox(100,100);
    //input.proxy = b2MakeProxy(polygon.vertices, polygon.count, 0);
    //input.translation = { 0,1000 };
    //input.translation = b2Body_GetPosition(id);
    b2ShapeProxy p = b2MakeOffsetProxy(polygon.vertices, polygon.count, 0,b2Body_GetPosition(id)+b2Vec2{1,1}, b2Body_GetRotation(id));
    b2World_CastShape(worldId, &p, {0,1000}, b2DefaultQueryFilter(), CastCallback, &context);

    sf::Vertex line2[]
    {

        sf::Vertex(b2Vec2_to_sfVector2f(b2Body_GetPosition(id)), sf::Color::Green),
        sf::Vertex(b2Vec2_to_sfVector2f(context.point),sf::Color::Green),
    };

    sf::Vertex line1[]
    {

        sf::Vertex(b2Vec2_to_sfVector2f(b2Body_GetWorldPoint(id, b2Vec2{0,0})),sf::Color::Red),
        sf::Vertex(b2Vec2_to_sfVector2f(b2Body_GetPosition(id) + b2Vec2{0, 300}),sf::Color::Red),
    };


    window.draw(line1, 2, sf::Lines);
    window.draw(line2, 2, sf::Lines);
}
void movePlayerEvents(b2BodyId id, sf::Event event) {
    if (event.key.code == sf::Keyboard::Key::Space) {
        b2Body_ApplyLinearImpulseToCenter(id, { 0,-10 }, true);
    }
}

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 8.25f };
    worldId = b2CreateWorld(&worldDef);
    // Making moving box
    sf::RectangleShape testShape{};
    b2BodyId bodyId{};
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 1.f;
    makeBox(testShape, bodyId, shapeDef, sf::Vector2f(50, 4.0f), sf::Vector2f(4.0f, 4.0f), b2_dynamicBody);

    sf::RectangleShape movingBox{};
    b2BodyId movingBoxId{};
    makeBox(movingBox, movingBoxId, b2DefaultShapeDef(), sf::Vector2f(50, 100), sf::Vector2f(32, 32), b2_dynamicBody);

    // Making floor
    sf::RectangleShape floor{};
    b2BodyId groundId{};
    makeBox(floor, groundId, b2DefaultShapeDef(), sf::Vector2f(50, 500), sf::Vector2f(50, 10), b2_staticBody);

    //b2Body_SetAngularVelocity(movingBoxId, 100000);

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
                movePlayerEvents(movingBoxId, event);
            }
        }
        Time currentTime = clock.getElapsedTime();

        movePlayer(movingBoxId);
        int subStepCount = 4;
        b2World_Step(worldId, 1. / 165, subStepCount);
        move(testShape, bodyId);
        move(movingBox, movingBoxId);

        window.setView(view);
        window.clear();
        window.draw(testShape);
        window.draw(floor);
        window.draw(movingBox);
        //onGround(movingBoxId, window);
        onGroundBroken(movingBoxId,window);
        window.display();
    }
    return 0;
}