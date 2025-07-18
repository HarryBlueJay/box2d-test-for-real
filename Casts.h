#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

using namespace sf;
bool showCasts = true;
float scaleFactor = 1.0f / 32.0f; // multiple of 2 to avoid precision issues
//also anything below 4 pixels causes trouble, don't expect to reach that
b2WorldId worldId;

//vec2forSFML
sf::Vector2f b2Vec2_to_sfVector2f(b2Vec2 input) {
    return sf::Vector2f(input.x, input.y);
}
b2Vec2 sfVector2f_to_b2Vec2(sf::Vector2f input) {
    return b2Vec2{ input.x, input.y };
}

//callbacks go here
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
    result->bodyId = b2Shape_GetBody(shapeId);
    result->fraction = fraction;
    result->hit = true;
    return fraction;
}
struct OverlapResult
{
    bool hit;
    int hits;
    int maxHits = 10;
    b2ShapeId hitIds[10];
};
bool OverlapCallback(b2ShapeId id, void* context) {
    OverlapResult* result = (OverlapResult*)context;
    result->hits += 1;
    result->hit = true;
    result->hitIds[result->hits] = id;
    return true;
}

void simpleLinecast(b2Vec2 point1, b2Vec2 point2, b2Vec2 offset, RenderWindow& window) {
    CastResult context = {};
    b2Vec2 points[2] = { point1, point2 };
    b2ShapeProxy p = b2MakeProxy(points, 2, 0.5);
    b2World_CastShape(worldId, &p, offset * 0.5, b2DefaultQueryFilter(), CastCallback, &context);
    sf::Vertex line1[]{
        sf::Vertex(b2Vec2_to_sfVector2f(point1),sf::Color::Red),
        sf::Vertex(b2Vec2_to_sfVector2f(point2),sf::Color::Red),
        sf::Vertex(b2Vec2_to_sfVector2f(point2 + offset),sf::Color::Red),
        sf::Vertex(b2Vec2_to_sfVector2f(point1 + offset),sf::Color::Red),
        sf::Vertex(b2Vec2_to_sfVector2f(point1),sf::Color::Red)
    };

    if (showCasts) {
        window.draw(line1, 5, sf::LineStrip);
    }
    if (context.hit) {
        sf::Vertex line2[]{
            sf::Vertex(b2Vec2_to_sfVector2f(point1), sf::Color::Green),
            sf::Vertex(b2Vec2_to_sfVector2f(context.point),sf::Color::Green),
        };
        if (showCasts) {
            window.draw(line2, 2, sf::Lines);
        }
    }
}

b2RayResult simpleRaycast(b2Vec2 start, b2Vec2 offset, RenderWindow& window)
{
    b2RayResult result = b2World_CastRayClosest(worldId, start, offset, b2DefaultQueryFilter());
    sf::Vertex line1[]{
        sf::Vertex(b2Vec2_to_sfVector2f(start),sf::Color::Red),
        sf::Vertex(b2Vec2_to_sfVector2f(start + offset),sf::Color::Red),
    };

    if (showCasts) {
        window.draw(line1, 2, sf::Lines);
    }
    if (result.hit) {
        sf::Vertex line2[]{
            sf::Vertex(b2Vec2_to_sfVector2f(start), sf::Color::Green),
            sf::Vertex(b2Vec2_to_sfVector2f(result.point),sf::Color::Green),
        };
        if (showCasts) {
            window.draw(line2, 2, sf::Lines);
        }
    }
    return result;
}

OverlapResult lineOverlap(b2Vec2 start, b2Vec2 end, b2QueryFilter filter, RenderWindow& window) {
    OverlapResult result = {};
    b2Vec2 points[2] = { start, end };
    b2ShapeProxy p = b2MakeProxy(points, 2, 0);
    b2World_OverlapShape(worldId, &p, filter, OverlapCallback, &result);
    sf::Color color = sf::Color::Red;
    if (result.hits > 0) {
        color = sf::Color::Green;
    }
    if (showCasts) {
        sf::Vertex line1[]{
            sf::Vertex(b2Vec2_to_sfVector2f(start),color),
            sf::Vertex(b2Vec2_to_sfVector2f(end),color),
        };
        window.draw(line1, 2, sf::Lines);
    }

    return result;
}

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
    makeBoxWithBodyDef(box, id, shapeDef, position, size, bodyDef);
}