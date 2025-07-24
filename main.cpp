#include <iostream>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Casts.h"
#include "Player.h"
#include "Level.h"
#include <cmath>
#include <math.h>

using namespace sf;
const int WINDOWWIDTH = 800;
const int WINDOWHEIGHT = 600;
b2WorldDef worldDef;

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 8.25f };
    worldId = b2CreateWorld(&worldDef);
    // Making moving box
    sf::RectangleShape testShape{};
    b2BodyId bodyId{};
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 1.f;
    makeBox(testShape, bodyId, shapeDef, sf::Vector2f(50, 4.0f), sf::Vector2f(4.0f, 4.0f), b2_dynamicBody);

    sf::RectangleShape playerBox{};
    b2BodyId playerBoxId{};
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    makeBoxWithBodyDef(playerBox, playerBoxId, b2DefaultShapeDef(), sf::Vector2f(50, 100), sf::Vector2f(64, 64), bodyDef);

    // Making floor
    sf::RectangleShape floor{};
    b2BodyId groundId{};
    makeBox(floor, groundId, b2DefaultShapeDef(), sf::Vector2f(50, 500), sf::Vector2f(50, 10), b2_staticBody);

    //b2Body_SetAngularVelocity(playerBoxId, 100000);

    RenderWindow window(VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Hello Physics");
    sf::View view(sf::FloatRect({ 0,0 }, { pixelsToMeters(WINDOWWIDTH) , pixelsToMeters(WINDOWHEIGHT) }));
    Player player(playerBoxId);
    Level::loadLevel("level1.json");

    Clock clock;
    Time lastTime = clock.getElapsedTime();
    window.setFramerateLimit(165);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
                player.movePlayerEvents(event);
            else if (event.type == sf::Event::Resized) {
                view.setSize({
                        static_cast<float>(pixelsToMeters(event.size.width)),
                        static_cast<float>(pixelsToMeters(event.size.height))
                    });
                // source: https://stackoverflow.com/questions/61447069/sfml-window-resizing-is-very-ugly
            }
            
        }
        Time currentTime = clock.getElapsedTime();

        window.setView(view);
        window.clear();

        float deltaTime = clock.restart().asSeconds();
        player.update(window, deltaTime);
        // view stuff
        sf::Vector2f viewPosition = view.getCenter();
        b2Vec2 result = b2Lerp({ viewPosition.x, viewPosition.y }, { playerBox.getPosition().x, playerBox.getPosition().y }, 0.1f);
        viewPosition.x = result.x;
        viewPosition.y = std::min(result.y, 0 - view.getSize().y + 33.75f + view.getSize().y / 2);
        view.setCenter(viewPosition);
        //
        move(testShape, bodyId);
        move(playerBox, playerBoxId);
        int subStepCount = 4;
        b2World_Step(worldId, deltaTime, subStepCount);

        for (LevelRectangle rectangle : Level::currentLevel) {
            std::cout << rectangle.rectangle.getPosition().x << " " << rectangle.rectangle.getPosition().x << std::endl;
            window.draw(rectangle.rectangle);
        }
        window.draw(testShape);
        window.draw(floor);
        window.draw(playerBox);
        window.display();
    }
    return 0;
}