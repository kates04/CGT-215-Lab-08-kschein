// CGT-215-Lab-08-kschein.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    SoundBuffer popBuffer;
    if (!popBuffer.loadFromFile("sound/balloonpop.ogg")) {
        cout << "coul not load balloonpop.ogg" << endl;
        exit(5);
    }
    Sound popSound;
    popSound.setBuffer(popBuffer);

    Music music;
    if (!music.openFromFile("sound/circus.ogg")) {
        cout << "Failed to load circus.ogg ";
        exit(6);
    }
    music.play();

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;


    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Font fnt;
    if (!fnt.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    long duckMs(0);
    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            duckMs = duckMs + deltaMS; //increment duck counter
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            //check if ducks go past the right edge of the screen and if so , remove them from the world and the ducks list
            for (PhysicsSprite& duck : ducks) {
                if (duck.getCenter().x + duck.getSize().x / 2 >= 790) {
                    world.RemovePhysicsBody(duck);
                    ducks.QueueRemove(duck);
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }

            //spawn ducks
            if (duckMs > 1000) { //if more than 1 second has passed, move the ducks
                duckMs = 0;
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(redTex);
                Vector2f sz = duck.getSize();
                //set center of duck to left side of screen
                duck.setCenter(Vector2f(0 + (sz.x / 2), 20 + (sz.y / 2)));
                duck.setVelocity(Vector2f(0.25, 0));

                world.AddPhysicsBody(duck);
                duck.onCollision =
                    [&drawingArrow, &world, &arrow, &duck, &ducks, &score, &popSound]
                    (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        popSound.play();
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                    };

            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            ducks.DoRemovals();
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }

            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            //set score bottom right corner of screen
            scoreText.setPosition(Vector2f(780 - GetTextSize(scoreText).x, 580 - GetTextSize(scoreText).y));
            window.draw(scoreText);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            //set arrow count bottom left corner of screen
            arrowCountText.setPosition(Vector2f(30 - GetTextSize(arrowCountText).x, 580 - GetTextSize(arrowCountText).y));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();

        }
    }
    window.display(); // this is needed to see the last frame
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (!Keyboard::isKeyPressed(Keyboard::Space)) {
    }
    window.close();
}