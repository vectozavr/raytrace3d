//
// Created by Иван Ильин on 14.01.2021.
//

#include <iostream>

#include <Engine.h>
#include <utils/Time.h>
#include <utils/ResourceManager.h>
#include <animation/Timeline.h>
#include <io/Keyboard.h>
#include <io/Mouse.h>
#include <cmath>

Engine::Engine() {
    Time::init();
    Timeline::init();
    Keyboard::init();
    Mouse::init();
    ResourceManager::init();
}

void Engine::create(uint16_t screenWidth, uint16_t screenHeight, const std::string &name, const Color& background) {

    _name = name;
    screen->open(screenWidth, screenHeight, name, background);

    Log::log("Engine::create(): started engine (" + std::to_string(screenWidth) + "x" + std::to_string(screenHeight) +
             ") with title '" + name + "'.");
    Time::update();

    start();
    camera->init(screenWidth, screenHeight);

    SDL_Init(SDL_INIT_EVERYTHING);

    while (screen->isOpen()) {
        // TODO: move event handling into another place
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    screen->close();
                    exit();
                    break;
                case SDL_KEYDOWN:
                    Keyboard::sendKeyboardEvent(e);
                    break;
                case SDL_KEYUP:
                    Keyboard::sendKeyboardEvent(e);
                    break;
                case SDL_MOUSEMOTION:
                    Mouse::sendMouseEvent(e);
                    break;
            }
        }

        // 'd' in the beginning of the name means debug.
        // While printing debug info we will take into account only timer names witch start with 'd '
        Time::startTimer("d all");

        screen->clear();

        Time::update();

        Time::startTimer("d game update");
        update();
        Time::stopTimer("d game update");

        if(_updateWorld) {
            Time::startTimer("d animations");
            Timeline::update();
            Time::stopTimer("d animations");

            Time::startTimer("d collisions");
            world->update();
            Time::stopTimer("d collisions");
        }

        Time::startTimer("d proj");
        // clear triangles from previous frame

        camera->clear();
        // project triangles to the camera plane
        for (auto &it : *world) {
            std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(it.second);
            if(mesh) {
                camera->project(mesh);
            }
        }

        Time::stopTimer("d proj");

        Time::startTimer("d depthBuffer");
        // draw triangles on the screen
        screen->clearDepthBuffer();
        Time::stopTimer("d depthBuffer");

        Time::startTimer("d render");
        for (auto &t : camera->sorted()) {
            screen->drawTriangle(*t.first, t.second);
        }
        Time::stopTimer("d render");

        // Ray tracing routine
        /*
        double a = (double)screenWidth/screenHeight;
        for(int j = 0; j < screenHeight; j++) {
            for(int i = 0; i < screenWidth; i++) {
                double x = a*(1.0/2.0 - (1.0*i + 0.5)/screenWidth);
                double y = 1*(1.0/2.0 - (1.0*j + 0.5)/screenHeight);
                Vec3D camera_rd(x, y,1);

                Vec3D world_rd = camera->model()*camera_rd;

                auto intersection = world->rayCast(camera->position(), camera->position() + world_rd);

                auto color = world->getIllumination(intersection, camera->position(), world_rd);

                if(intersection.distanceToObject < std::numeric_limits<double>::infinity()) {
                    screen->drawPixel(i, j, color/(1 + intersection.distanceToObject/50));
                } else {
                    screen->drawPixel(i, j, Color(0, 0, 0, 255));
                }
            }
        }
        */

        printDebugInfo();

        screen->display();

        Time::stopTimer("d all");
    }
}

void Engine::exit() {
    if (screen->isOpen()) {
        screen->close();
    }

    Time::free();
    Timeline::free();
    Keyboard::free();
    Mouse::free();
    ResourceManager::free();

    Log::log("Engine::exit(): exit engine (" + std::to_string(screen->width()) + "x" +
             std::to_string(screen->height()) + ") with title '" + screen->title() + "'.");
}

void Engine::printDebugInfo() const {
    if (_showDebugInfo) {
        // coordinates & fps:
        std::string text = _name + "\n\n X: " +
                           std::to_string((camera->position().x())) + "\n Y: " +
                           std::to_string((camera->position().y())) + "\n Z: " +
                           std::to_string((camera->position().z())) + "\n RY:" +
                            std::to_string(camera->angle().y()) + "\n RL: " +
                            std::to_string(camera->angleLeftUpLookAt().x()) + "\n\n" +
                           std::to_string(screen->width()) + "x" +
                           std::to_string(screen->height()) + "\t" +
                           std::to_string(Time::fps()) + " fps";


        std::cout << text << std::endl;

        // timers:
        int timerWidth = 0.9*screen->width();
        float xPos = screen->width()/10;
        float yPos = screen->height()/2;
        int height = screen->height()/30;

        double totalTime = Time::elapsedTimerSeconds("d all");
        double timeSum = 0;
        int i = 0;

        if(!Time::timers()) {
            return;
        }
        for (auto &[timerName, timer] : Time::timers()->get()) {
            auto ttt = timerName;

            int width = timerWidth * timer.elapsedSeconds() / totalTime;

            if (timerName == "d all" || timerName[0] != 'd') {
                continue;
            }

            screen->drawStrokeRectangle(xPos, yPos + (1.5*height)*i, width, height,
                                        Color( {(float)(width) / timerWidth, (1.0f - (float)(width) / timerWidth), 0, 1}));
            /*
            screen->drawText(
                    timerName.substr(2, timerName.size()) + ":\t" + std::to_string(timer.elapsedMilliseconds()) + " ms \t (" +
                    std::to_string((int) (100 * timer.elapsedSeconds() / totalTime)) + "%)",
                    Vec2D{xPos + 10, yPos + height * i + 5}, 30,
                    sf::Color(0, 0, 0, 150));
            */

            std::cout << timerName << std::endl;

            i++;
            timeSum += timer.elapsedSeconds();
        }

        int width = timerWidth * (totalTime - timeSum) / totalTime;
        screen->drawStrokeRectangle(xPos, yPos + (1.5*height)*i, width, height,
                             Color( {(float)(width) / timerWidth, (1.0f - (float)(width) / timerWidth), 0, 1}));
        /*
        screen->drawText("other:\t" + std::to_string(1000*(totalTime - timeSum)) + " ms \t (" +
                         std::to_string((int) (100 * (totalTime - timeSum) / totalTime)) + "%)",
                         Vec2D{xPos + 10, yPos + height * i + 5}, 30,
                         sf::Color(0, 0, 0, 150));
                         */
    }
}
