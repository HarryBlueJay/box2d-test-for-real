#include "BaseCollider.h"
#include "Casts.h"

BaseCollider::BaseCollider(sf::Transformable* _transform, float _parallaxFactor, b2BodyId _bodyId):
    DrawableObject(_transform, _parallaxFactor),
    bodyId(_bodyId) {
    move();
}
void BaseCollider::move() {
    transform->setPosition(Casts::get().b2Vec2_to_sfVector2f(b2Body_GetPosition(bodyId)));
    transform->setRotation(sf::radians(b2Rot_GetAngle(b2Body_GetRotation(bodyId))));
}