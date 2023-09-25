//
// Created by Иван Ильин on 14.01.2021.
//

#include <utils/Time.h>
#include <animation/Timeline.h>
#include <Engine.h>

#include <iostream>

Engine::Engine() {
    Time::init();
    Timeline::init();
}

void Engine::create(uint16_t screenWidth, uint16_t screenHeight, const std::string &name, const Color& background) {
    _name = name;
    screen->open(screenWidth, screenHeight, name, background);

    Log::log("Engine::create(): started engine (" + std::to_string(screenWidth) + "x" + std::to_string(screenHeight) +
             ") with title '" + name + "'.");
    Time::update();

    start();
    camera->init(screenWidth, screenHeight);

    while (screen->isOpen()) {
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

        Time::startTimer("d projections");

        // clear triangles from previous frame
        camera->clear();
        // project triangles to the camera plane
        for (auto &it : *world) {
            camera->project(it.second);
        }
        // draw triangles on the screen
        for (auto &t : camera->sorted()) {
            screen->drawTriangle(*t);
        }

        Time::stopTimer("d projections");

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
                            std::to_string(camera->angle().y()/Consts::PI) + "PI\n RL: " +
                            std::to_string(camera->angleLeftUpLookAt().x()/Consts::PI) + "PI\n\n" +
                           std::to_string(screen->width()) + "x" +
                           std::to_string(screen->height()) + "\t" +
                           std::to_string(Time::fps()) + " fps";


        std::cout << text << std::endl;
    }
}
