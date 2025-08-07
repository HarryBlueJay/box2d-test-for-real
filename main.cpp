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

    sf::RectangleShape playerBox{};
    b2BodyId playerBoxId{};
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    b2ShapeDef playerShapeDef = b2DefaultShapeDef();
    b2SurfaceMaterial playerMaterial = b2DefaultSurfaceMaterial();
    playerShapeDef.density /= 4;
    playerMaterial.friction = 0;
    playerShapeDef.material = playerMaterial;
    makeBoxWithBodyDef(playerBox, playerBoxId, playerShapeDef, sf::Vector2f(50, 100), sf::Vector2f(64, 64), bodyDef);

    // Making floor
    sf::RectangleShape floor{};
    b2BodyId groundId{};
    makeBox(floor, groundId, b2DefaultShapeDef(), sf::Vector2f(50, 500), sf::Vector2f(50, 10), b2_staticBody);

    //b2Body_SetAngularVelocity(playerBoxId, 100000);
    sf::ConvexShape test(3);
    test.setPoint(0,sf::Vector2f(-13, -39));
    test.setPoint(1, sf::Vector2f(-10, -40));
    test.setPoint(2, sf::Vector2f(-5, -41));
    test.setFillColor(sf::Color::Black);

    RenderWindow window(VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Hello Physics");
    sf::View view(sf::FloatRect({ 0,0 }, { pixelsToMeters(WINDOWWIDTH) , pixelsToMeters(WINDOWHEIGHT) }));
    Player player(playerBoxId,playerBox);
    Level::loadLevel("level1.json");
    player.die();

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
        window.clear(sf::Color::White);

        float deltaTime = clock.restart().asSeconds();
        player.update(window, deltaTime);
        // view stuff
        sf::Vector2f viewPosition = view.getCenter();
        b2Vec2 result = b2Lerp({ viewPosition.x, viewPosition.y }, { playerBox.getPosition().x, playerBox.getPosition().y }, 0.1f);
        viewPosition.x = result.x;
        viewPosition.y = std::min(result.y, 0 - view.getSize().y + 33.75f + view.getSize().y / 2);
        view.setCenter(viewPosition);
        move(playerBox, playerBoxId);
        int subStepCount = 4;
        b2World_Step(worldId, deltaTime, subStepCount);

        for (LevelRectangle rectangle : Level::currentLevel) {
            window.draw(rectangle.rectangle);
        }
        window.draw(test);
        window.draw(floor);
        player.draw(window);
        window.display();
    }
    return 0;
}