#include "BasicIncludes.h"
#include "Casts.h"
#include "Player.h"
#include "Level.h"
#include "TransitionManager.h"
#include <math.h>
#include <Windows.h>

b2WorldDef worldDef;
b2WorldId worldId;

std::vector<Object*> objectList;

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 40.0f };
    Level::loadLevelList();
    Level::loadLevel(0);
    
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Black World");

    ShowWindow(window.getSystemHandle(),SW_MAXIMIZE);
    sf::Clock clock;
    sf::Time lastTime = clock.getElapsedTime();
    window.setFramerateLimit(240);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
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
            TransitionManager::get().update(window, deltaTime);
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
            // Update objects
            for (int i = 0; i < objectList.size(); i++) {
                objectList[i]->update(window, deltaTime);
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