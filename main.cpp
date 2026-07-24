#include "BasicIncludes.h"
#include "Casts.h"
#include "Player.h"
#include "Level.h"
#include "TransitionManager.h"
#include <math.h>
#include <Windows.h>

b2WorldDef worldDef;
b2WorldId worldId;
sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Black World");

std::vector<Object*> objectList;

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
        if (TransitionManager::get().isTransitioning()) {
            TransitionManager::get().update(deltaTime);
        }
        else {
            //Collision
            for (int i = 0; i < objectList.size(); i++) {
                BaseCollider* collider = dynamic_cast<BaseCollider*>(objectList[i]);
                if (!collider) { continue; }
                b2ContactData data[10];
                int elements = b2Body_GetContactData(collider->bodyId, data, 10);
                for (int j = 0; j < elements; j++) {
                    b2ContactData contactData = data[j];
                    b2BodyId bodyA = b2Shape_GetBody(contactData.shapeIdA);
                    b2BodyId bodyB = b2Shape_GetBody(contactData.shapeIdB);
                    int k;
                    b2Vec2 normal = contactData.manifold.normal;
                    if (bodyA.index1 == collider->bodyId.index1) {
                        k = reinterpret_cast<int>(b2Body_GetUserData(bodyB));
                        normal *= -1;
                    }
                    else {
                        k = reinterpret_cast<int>(b2Body_GetUserData(bodyA));
                    }
                    collider->collide(objectList[k], normal);
                }
            }
            // Sensors
            b2SensorEvents sensorEvents = b2World_GetSensorEvents(worldId);
            for (int i = 0; i < sensorEvents.beginCount; ++i) {
                b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
                int colliderIndex = reinterpret_cast<int>(b2Body_GetUserData(b2Shape_GetBody(beginTouch->visitorShapeId)));
                int sensorIndex = reinterpret_cast<int>(b2Body_GetUserData(b2Shape_GetBody(beginTouch->sensorShapeId)));
                BaseCollider* collider = dynamic_cast<BaseCollider*>(objectList[colliderIndex]);
                collider->touch(objectList[sensorIndex]);
                // process begin event
            }
            // Update objects
            for (int i = 0; i < objectList.size(); i++) {
                objectList[i]->update(deltaTime);
            }

            int subStepCount = 4;
            b2World_Step(worldId, deltaTime, subStepCount);
        }

        for (int i = 0; i < objectList.size(); i++) {
            objectList[i]->draw(window);
        }
        TransitionManager::get().draw(window);
        window.display();
    }
    return 0;
}