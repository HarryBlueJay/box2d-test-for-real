#include "BasicIncludes.h"
#include "Casts.h"
#include "BaseCollider.h"
#include "Level.h"
extern b2WorldId worldId;
bool showCasts = true;

//vec2forSFML
sf::Vector2f Casts::b2Vec2_to_sfVector2f(b2Vec2 input) {
    return sf::Vector2f(input.x, input.y);
}
b2Vec2 Casts::sfVector2f_to_b2Vec2(sf::Vector2f input) {
    return b2Vec2{ input.x, input.y };
}
sf::Vector2f Casts::rotate(sf::Vector2f input, float radians) {
    float sin = std::sin(radians);
    float cos = std::cos(radians);
    return sf::Vector2f(
        input.x * cos - input.y * sin,
        input.x * sin + input.y * cos
    );
}

//callbacks go here
float CastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
    Casts::CastResult* result = (Casts::CastResult*)context;
    result->normal = normal;
    result->point = point;
    result->bodyId = b2Shape_GetBody(shapeId);
    result->fraction = fraction;
    result->hit = true;
    return fraction;
}
bool OverlapCallback(b2ShapeId id, void* context) {
    Casts::OverlapResult* result = (Casts::OverlapResult*)context;
    result->hits += 1;
    result->hit = true;
    result->hitIds[result->hits] = id;
    return true;
}

Casts::CastResult Casts::circlecast(b2Vec2 point, float radius, b2Vec2 offset) {
    CastResult context = {};
    b2Vec2 points[1] = { point };
    b2ShapeProxy p = b2MakeProxy(points, 1, radius);
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.maskBits = CollisionGroup::LEVEL;
    b2World_CastShape(worldId, &p, offset, filter, CastCallback, &context);

    if (showCasts && context.hit) {
        sf::CircleShape* circle = new sf::CircleShape(radius, 8);
        circle->setFillColor(sf::Color::Transparent);
        circle->setOutlineColor(sf::Color::Red);
        circle->setOutlineThickness(-scaleFactor);
        circle->setPosition(b2Vec2_to_sfVector2f(context.point + context.normal * radius - b2Vec2{ radius, radius }));
        shapes.push_back(circle);

        sf::VertexArray* line = new sf::VertexArray(sf::PrimitiveType::Lines, 2);
        (*line)[0] = { b2Vec2_to_sfVector2f(point), sf::Color::Green };
        (*line)[1] = { b2Vec2_to_sfVector2f(context.point),sf::Color::Green };
        shapes.push_back(line);
    }
    return context;
}

b2RayResult Casts::simpleRaycast(b2Vec2 start, b2Vec2 offset)
{
    b2RayResult result = b2World_CastRayClosest(worldId, start, offset, b2DefaultQueryFilter());
    if (showCasts) {
        if (result.hit) {
            sf::VertexArray* line = new sf::VertexArray(sf::PrimitiveType::Lines, 2);
            (*line)[0] = { b2Vec2_to_sfVector2f(start), sf::Color::Green };
            (*line)[1] = { b2Vec2_to_sfVector2f(result.point),sf::Color::Green };
            shapes.push_back(line);
        }
        else {
            sf::VertexArray* line = new sf::VertexArray(sf::PrimitiveType::Lines, 2);
            (*line)[0] = { b2Vec2_to_sfVector2f(start),sf::Color::Red };
            (*line)[1] = { b2Vec2_to_sfVector2f(start + offset),sf::Color::Red };
            shapes.push_back(line);
        }
    }
    return result;
}

Casts::OverlapResult Casts::lineOverlap(b2Vec2 start, b2Vec2 end, b2QueryFilter filter) {
    OverlapResult result = {};
    b2Vec2 points[2] = { start, end };
    b2ShapeProxy p = b2MakeProxy(points, 2, 0);
    b2World_OverlapShape(worldId, &p, filter, OverlapCallback, &result);
    if (showCasts) {
        sf::Color color = sf::Color::Red;
        if (result.hits > 0) {
            color = sf::Color::Green;
        }
        sf::VertexArray* line = new sf::VertexArray(sf::PrimitiveType::Lines, 2);
        (*line)[0] = { b2Vec2_to_sfVector2f(start), color };
        (*line)[1] = { b2Vec2_to_sfVector2f(end), color };
        shapes.push_back(line);
    }

    return result;
}

void Casts::draw(sf::RenderWindow& window) {
    if (!showCasts) { return; }
    for (int i = 0; i < shapes.size(); i++) {
        window.draw(*shapes[i]);
        delete shapes[i];
    }
    shapes.clear();
}