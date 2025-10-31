#include "BasicIncludes.h"
#include "Casts.h"
#include "Player.h"
#include "Level.h"
#include <math.h>

const int WINDOWWIDTH = 800;
const int WINDOWHEIGHT = 600;
b2WorldDef worldDef;
b2WorldId worldId;

extern std::vector<LevelRectangle> currentLevel;
std::vector<Object*> objectList;

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 40.0f };
    Level::loadLevelList();
    Level::loadLevel(0);
    
    sf::RenderWindow window(sf::VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Hello Physics");
    sf::View view(sf::FloatRect({ 0,0 }, { Casts::pixelsToMeters(WINDOWWIDTH) , Casts::pixelsToMeters(WINDOWHEIGHT) }));

    sf::Clock clock;
    sf::Time lastTime = clock.getElapsedTime();
    window.setFramerateLimit(240);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                view.setSize({
                        static_cast<float>(Casts::pixelsToMeters(event.size.width)),
                        static_cast<float>(Casts::pixelsToMeters(event.size.height))
                    });
                // source: https://stackoverflow.com/questions/61447069/sfml-window-resizing-is-very-ugly
            }
            else {
                for (int i = 0; i < objectList.size(); i++) {
                    objectList[i]->inputCallback(event);
                }
            }
        }
        sf::Time currentTime = clock.getElapsedTime();

        window.setView(view);
        window.clear(sf::Color::White);
        for (int i = 0; i < objectList.size(); i++) {
            BaseCollider* collider = dynamic_cast<BaseCollider*>(objectList[i]);
            if (!collider) { continue; }
            b2ContactData data[10];
            int elements = b2Body_GetContactData(collider->bodyId, data, 10);
            for (int j = 0; j < elements; j++) {
                b2ContactData contactData = data[j];
                b2BodyId bodyA = b2Shape_GetBody(contactData.shapeIdA);
                b2BodyId bodyB = b2Shape_GetBody(contactData.shapeIdB);
                if (bodyA.index1 == collider->bodyId.index1) {
                    collider->collide(bodyB, -contactData.manifold.normal);
                }
                else {
                    collider->collide(bodyA, contactData.manifold.normal);
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        for (int i = 0; i < objectList.size(); i++) {
            objectList[i]->update(window, deltaTime);
        }

        // view stuff
        Player* player = nullptr;
        for (int i = 0; i < objectList.size(); i++) {
            player = dynamic_cast<Player*>(objectList[i]);
            if (player) {
                sf::Vector2f viewPosition = view.getCenter();
                b2Vec2 start = { viewPosition.x, viewPosition.y };
                sf::Vector2f playerPosition = player->rectangle.getPosition();
                b2Vec2 end = { playerPosition.x, playerPosition.y };
                b2Vec2 result = end + (start - end) * std::exp(-deltaTime * 15);

                viewPosition.x = result.x;
                viewPosition.y = result.y;
                view.setCenter(viewPosition);
                break;
            }
        }

        int subStepCount = 4;
        b2World_Step(worldId, deltaTime, subStepCount);

        for (LevelRectangle rectangle : currentLevel) {
            window.draw(rectangle.rectangle);
        }
        for (int i = 0; i < objectList.size(); i++) {
            objectList[i]->draw(window);
        }
        window.display();
    }
    return 0;
}