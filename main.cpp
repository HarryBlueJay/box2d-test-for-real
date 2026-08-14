#include "BasicIncludes.h"
#include "Casts.h"
#include "Player.h"
#include "Level.h"
#include "TransitionManager.h"
#include "Camera.h"
#include <math.h>
#include <Windows.h>

b2WorldDef worldDef;
b2WorldId worldId;
sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Black World");

std::vector<Object*> objectList;
const std::vector<Object*> singletonList = { &TransitionManager::get(), &Camera::get(), &Casts::get() };

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 40.0f };
    Level::loadLevelList();
    Level::loadLevel(0);

    ShowWindow(window.getNativeHandle(),SW_MAXIMIZE);
    window.setKeyRepeatEnabled(false);
    sf::Clock clock;
    sf::Time lastTime = clock.getElapsedTime();
    window.setFramerateLimit(240);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else {
                for (int i = 0; i < objectList.size(); i++) {
                    objectList[i]->inputCallback(event);
                }
            }
        }
        sf::Time currentTime = clock.getElapsedTime();
        float deltaTime = clock.restart().asSeconds();

        window.clear(sf::Color::White);
        if (!TransitionManager::get().isTransitioning()) {
            // Sensors
            b2SensorEvents sensorEvents = b2World_GetSensorEvents(worldId);
            for (int i = 0; i < sensorEvents.beginCount; ++i) {
                b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
                int colliderIndex = reinterpret_cast<int>(b2Body_GetUserData(b2Shape_GetBody(beginTouch->visitorShapeId)));
                int sensorIndex = reinterpret_cast<int>(b2Body_GetUserData(b2Shape_GetBody(beginTouch->sensorShapeId)));
                BaseCollider* collider = dynamic_cast<BaseCollider*>(objectList[colliderIndex]);
                collider->touch(objectList[sensorIndex]);

            }
            // Collision
            b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);
            for (int i = 0; i < contactEvents.beginCount; ++i) {
                b2ContactBeginTouchEvent* beginTouch = contactEvents.beginEvents + i;
                int colliderIndex = reinterpret_cast<int>(b2Body_GetUserData(b2Shape_GetBody(beginTouch->shapeIdA)));
                int otherIndex = reinterpret_cast<int>(b2Body_GetUserData(b2Shape_GetBody(beginTouch->shapeIdB)));
                BaseCollider* collider = dynamic_cast<BaseCollider*>(objectList[colliderIndex]);
                BaseCollider* otherCollider = dynamic_cast<BaseCollider*>(objectList[otherIndex]);
                b2Vec2 normal = beginTouch->manifold.normal;
                collider->collide(objectList[otherIndex], -normal);
                otherCollider->collide(objectList[colliderIndex], normal);
            }
            // Update objects
            for (int i = 0; i < objectList.size(); i++) {
                objectList[i]->update(deltaTime);
            }

            int subStepCount = 4;
            b2World_Step(worldId, deltaTime, subStepCount);
        }
        // Update singletons
        for (int i = 0; i < singletonList.size(); i++) {
            singletonList[i]->update(deltaTime);
        }
        // Draw objects and singletons
        for (int i = 0; i < objectList.size(); i++) {
            objectList[i]->draw(window);
        }
        for (int i = 0; i < singletonList.size(); i++) {
            singletonList[i]->draw(window);
        }
        window.display();
    }
    return 0;
}