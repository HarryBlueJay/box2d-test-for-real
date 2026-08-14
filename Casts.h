#pragma once
#include "BasicIncludes.h"
#include "Singleton.h"

class Casts : public Singleton<Casts> {
private:
    std::vector<sf::Drawable*> shapes = {};
public:
    const float scaleFactor = 1.0f / 32.0f; // multiple of 2 to avoid precision issues
    //also anything below 4 pixels causes trouble, don't expect to reach that
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
    struct OverlapResult
    {
        bool hit;
        int hits;
        int maxHits = 10;
        b2ShapeId hitIds[10];
    };

    CastResult circlecast(b2Vec2 point, float radius, b2Vec2 offset);

    b2RayResult simpleRaycast(b2Vec2 start, b2Vec2 offset);

    OverlapResult lineOverlap(b2Vec2 start, b2Vec2 end, b2QueryFilter filter);

    template <typename T = float>
    T pixelsToMeters(T input) {
        return input * scaleFactor;
    };
    void move(sf::Transformable& rectangle, b2BodyId& id);

    void makeCircleWithBodyDef(sf::ConvexShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, float rotation, b2BodyDef bodyDef);
    // maybe make a version that takes in a body def
    void makeBox(sf::ConvexShape& box, b2BodyId* id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, float rotation, b2BodyType bodyType);
    void makePolygon(sf::ConvexShape& box, b2BodyId* id, b2ShapeDef shapeDef, std::vector<sf::Vector2f> offsets, sf::Vector2f position, float rotation, b2BodyType bodyType);

    void draw(sf::RenderWindow& window);
};