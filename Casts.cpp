#include "BasicIncludes.h"
#include "Casts.h"
#include "BaseCollider.h"
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
static float Casts::CastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
    CastResult* result = (CastResult*)context;
    result->point = point;
    result->bodyId = b2Shape_GetBody(shapeId);
    result->fraction = fraction;
    result->hit = true;
    return fraction;
}
bool Casts::OverlapCallback(b2ShapeId id, void* context) {
    OverlapResult* result = (OverlapResult*)context;
    result->hits += 1;
    result->hit = true;
    result->hitIds[result->hits] = id;
    return true;
}

void Casts::simpleLinecast(b2Vec2 point1, b2Vec2 point2, b2Vec2 offset, sf::RenderWindow& window) {
    CastResult context = {};
    b2Vec2 points[2] = { point1, point2 };
    b2ShapeProxy p = b2MakeProxy(points, 2, 0.5);
    b2World_CastShape(worldId, &p, offset * 0.5, b2DefaultQueryFilter(), CastCallback, &context);
    sf::Vertex line1[]{
        {b2Vec2_to_sfVector2f(point1),sf::Color::Red },
        {b2Vec2_to_sfVector2f(point2),sf::Color::Red },
        {b2Vec2_to_sfVector2f(point2 + offset),sf::Color::Red },
        {b2Vec2_to_sfVector2f(point1 + offset),sf::Color::Red },
        {b2Vec2_to_sfVector2f(point1),sf::Color::Red }
    };

    if (showCasts) {
        window.draw(line1, 5, sf::PrimitiveType::LineStrip);
    }
    if (context.hit) {
        sf::Vertex line2[]{
            { b2Vec2_to_sfVector2f(point1), sf::Color::Green },
            { b2Vec2_to_sfVector2f(context.point),sf::Color::Green },
        };
        if (showCasts) {
            window.draw(line2, 2, sf::PrimitiveType::Lines);
        }
    }
}

b2RayResult Casts::simpleRaycast(b2Vec2 start, b2Vec2 offset, sf::RenderWindow& window)
{
    b2RayResult result = b2World_CastRayClosest(worldId, start, offset, b2DefaultQueryFilter());
    sf::Vertex line1[]{
        { b2Vec2_to_sfVector2f(start),sf::Color::Red },
        { b2Vec2_to_sfVector2f(start + offset),sf::Color::Red },
    };

    if (showCasts) {
        window.draw(line1, 2, sf::PrimitiveType::Lines);
    }
    if (result.hit) {
        sf::Vertex line2[]{
            { b2Vec2_to_sfVector2f(start), sf::Color::Green },
            { b2Vec2_to_sfVector2f(result.point),sf::Color::Green },
        };
        if (showCasts) {
            window.draw(line2, 2, sf::PrimitiveType::Lines);
        }
    }
    return result;
}

Casts::OverlapResult Casts::lineOverlap(b2Vec2 start, b2Vec2 end, b2QueryFilter filter, sf::RenderWindow& window) {
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
            { b2Vec2_to_sfVector2f(start),color },
            { b2Vec2_to_sfVector2f(end),color },
        };
        window.draw(line1, 2, sf::PrimitiveType::Lines);
    }

    return result;
}

void Casts::move(sf::Transformable& rectangle, b2BodyId& id) {
    rectangle.setPosition(b2Vec2_to_sfVector2f(b2Body_GetPosition(id)));
    rectangle.setRotation(sf::radians(b2Rot_GetAngle(b2Body_GetRotation(id))));
}
void setupPolygon(b2Polygon& dynamicBox, b2BodyId& id, b2ShapeDef shapeDef, sf::ConvexShape& box, sf::Vector2f position, float rotation, b2BodyDef bodyDef) {
    position = Casts::pixelsToMeters(position);
    bodyDef.position = Casts::sfVector2f_to_b2Vec2(position);
    bodyDef.rotation = b2MakeRot(rotation * B2_PI / 180);
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    id = b2CreateBody(worldId, &bodyDef);
    b2CreatePolygonShape(id, &shapeDef, &dynamicBox);

    Casts::move(box, id);
}
void setupBox(sf::ConvexShape& box, sf::Vector2f size) {
    box.setPointCount(4);
    //sf::Vector2f halfSize = size / 2.0f;
    //box.setPoint(0, halfSize);
    //box.setPoint(1, sf::Vector2f(-halfSize.x, halfSize.y));
    //box.setPoint(2, -halfSize);
    //box.setPoint(3, sf::Vector2f(halfSize.x, -halfSize.y));
    box.setPoint(0, sf::Vector2f());
    box.setPoint(1, sf::Vector2f(size.x, 0));
    box.setPoint(2, size);
    box.setPoint(3, sf::Vector2f(0, size.y));
}
void Casts::makeCircleWithBodyDef(sf::ConvexShape& box, b2BodyId& id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, float rotation, b2BodyDef bodyDef) {
    position.x -= size.x;
    size = sf::Vector2f(pixelsToMeters(size.x), pixelsToMeters(size.y));
    float smallestEdge = fmin(size.x / 2, size.y / 2);
    float radius = smallestEdge * 0.9f;
    float edgeSize = smallestEdge - radius;
    setupBox(box, size);
    b2Vec2 offsets[4];
    for (int i = 0; i < 4; i++) {
        offsets[i] = b2Vec2{size.x/2.0f, size.y/2.0f};
        if (i < 2) {
            offsets[i].x -= edgeSize / 2;
        }
        else {
            offsets[i].x += edgeSize / 2;
        }
        if (i % 3 == 0) {
            offsets[i].y += edgeSize / 2;
        }
        else {
            offsets[i].y -= edgeSize / 2;
        }
    }
    b2Hull hull = b2ComputeHull(offsets, 4);
    b2Polygon polygon = b2MakePolygon(&hull, radius);
    setupPolygon(polygon, id, shapeDef, box, position, rotation, bodyDef);
}
// maybe make a version that takes in a body def
void Casts::makeBox(sf::ConvexShape& box, b2BodyId* id, b2ShapeDef shapeDef, sf::Vector2f position, sf::Vector2f size, float rotation, b2BodyType bodyType) {
    size = sf::Vector2f(pixelsToMeters(size.x), pixelsToMeters(size.y));
    setupBox(box, size);
    if (id) {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = bodyType;
        b2Vec2 offsets[4];
        for (int i = 0; i < box.getPointCount(); i++) {
            offsets[i] = sfVector2f_to_b2Vec2(box.getPoint(i));
        }
        b2Hull hull = b2ComputeHull(offsets, 4);
        b2Polygon polygon = b2MakePolygon(&hull, 0);
        setupPolygon(polygon, *id, shapeDef, box, position, rotation, bodyDef);
    }
}

void Casts::makePolygon(sf::ConvexShape& shape, b2BodyId* id, b2ShapeDef shapeDef, std::vector<sf::Vector2f> offsets, sf::Vector2f position, float rotation, b2BodyType bodyType) {
    shape.setPointCount(offsets.size());
    for (int i = 0; i < offsets.size(); i++) {
        shape.setPoint(i, offsets[i]);
    }
    if (id) {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = bodyType;
        b2Hull hull = b2ComputeHull(reinterpret_cast<const b2Vec2*>(&offsets[0]), offsets.size());
        //b2Polygon polygon = b2MakeOffsetPolygon(&hull, sfVector2f_to_b2Vec2(position), b2MakeRot(rotation * B2_PI / 180));
        b2Polygon polygon = b2MakePolygon(&hull, 0);
        setupPolygon(polygon, *id, shapeDef, shape, position, rotation, bodyDef);
    }
}