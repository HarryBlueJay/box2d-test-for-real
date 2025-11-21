#pragma once
#include "BasicIncludes.h"

namespace Casts { 
    //vec2forSFML
    sf::Vector2f b2Vec2_to_sfVector2f(b2Vec2 input);
    b2Vec2 sfVector2f_to_b2Vec2(sf::Vector2f input);
    sf::Vector2f rotate(sf::Vector2f input, float radians);

    //callbacks go here
    struct CastResult
    {
        b2Vec2 point;
        b2BodyId bodyId;
        float fraction;
        bool hit;
    };          
    static float CastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);
    struct OverlapResult
    {
        bool hit;
        int hits;
        int maxHits = 10;
        b2ShapeId hitIds[10];
    };
    bool OverlapCallback(b2ShapeId id, void* context);

    void simpleLinecast(b2Vec2 point1, b2Vec2 point2, b2Vec2 offset, sf::RenderWindow& window);

    b2RayResult simpleRaycast(b2Vec2 start, b2Vec2 offset, sf::RenderWindow& window);

    OverlapResult lineOverlap(b2Vec2 start, b2Vec2 end, b2QueryFilter filter, sf::RenderWindow& window);

    float pixelsToMeters(float input);
    void move(sf::RectangleShape& rectangle, b2BodyId& id);

    void makeBoxWithBodyDef(sf::RectangleShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, float rotation, b2BodyDef bodyDef);
    // maybe make a version that takes in a body def
    void makeBox(sf::RectangleShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, float rotation, b2BodyType bodyType);
}