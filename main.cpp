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

int main()
{
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 40.0f };
    Level::loadLevel("level1.json");

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
    Casts::makeBoxWithBodyDef(playerBox, playerBoxId, playerShapeDef, sf::Vector2f(50, 100), sf::Vector2f(64, 64), 0, bodyDef);

    //b2Body_SetAngularVelocity(playerBoxId, 100000);
    sf::ConvexShape test(3);
    test.setPoint(0,sf::Vector2f(-13, -39));
    test.setPoint(1, sf::Vector2f(-10, -40));
    test.setPoint(2, sf::Vector2f(-5, -41));
    test.setFillColor(sf::Color::Black);

    sf::RenderWindow window(sf::VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Hello Physics");
    sf::View view(sf::FloatRect({ 0,0 }, { Casts::pixelsToMeters(WINDOWWIDTH) , Casts::pixelsToMeters(WINDOWHEIGHT) }));
    Player player(playerBoxId,playerBox);
    player.die();

    sf::Clock clock;
    sf::Time lastTime = clock.getElapsedTime();
    window.setFramerateLimit(240);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
                player.movePlayerEvents(event);
            else if (event.type == sf::Event::Resized) {
                view.setSize({
                        static_cast<float>(Casts::pixelsToMeters(event.size.width)),
                        static_cast<float>(Casts::pixelsToMeters(event.size.height))
                    });
                // source: https://stackoverflow.com/questions/61447069/sfml-window-resizing-is-very-ugly
            }
        }
        sf::Time currentTime = clock.getElapsedTime();

        window.setView(view);
        window.clear(sf::Color::White);

        float deltaTime = clock.restart().asSeconds();
        player.update(window, deltaTime);

        // view stuff
        sf::Vector2f viewPosition = view.getCenter();
        b2Vec2 start = { viewPosition.x, viewPosition.y };
        b2Vec2 end = { playerBox.getPosition().x, playerBox.getPosition().y };
        b2Vec2 result = end + (start - end) * std::exp(-deltaTime*15);
        
        viewPosition.x = result.x;
        viewPosition.y = result.y;
        view.setCenter(viewPosition);

        Casts::move(playerBox, playerBoxId);
        int subStepCount = 4;
        b2World_Step(worldId, deltaTime, subStepCount);

        for (LevelRectangle rectangle : currentLevel) {
            // sfml rectangles rotate around center and not the top left, like tiled
            window.draw(rectangle.rectangle);
        }
        window.draw(test);
        player.draw(window);
        window.display();
    }
    return 0;
}