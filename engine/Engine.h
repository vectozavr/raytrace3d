//
// Created by Иван Ильин on 14.01.2021.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <io/Keyboard.h>
#include <io/Screen.h>
#include <io/Mouse.h>
#include <utils/Log.h>
#include <Camera.h>
#include <World.h>

class Engine {
private:
    std::string _name;

    int _triPerSec = 0;
    bool _updateWorld = true;
    bool _showDebugInfo = Consts::SHOW_DEBUG_INFO;

    void printDebugInfo() const;

protected:
    const std::shared_ptr<Screen> screen = std::make_shared<Screen>();
    const std::shared_ptr<Keyboard> keyboard = std::make_shared<Keyboard>();
    const std::shared_ptr<Mouse> mouse = std::make_shared<Mouse>(screen);

    const std::shared_ptr<World> world = std::make_shared<World>();
    const std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    virtual void start() {};

    virtual void update() {};

    [[nodiscard]] bool showDebugInfo() const { return _showDebugInfo; }
    void setDebugInfo(bool value) { _showDebugInfo = value; }

    void setUpdateWorld(bool value) { _updateWorld = value; }

    virtual void gui() {}

public:
    Engine();

    virtual ~Engine() = default;

    void create(uint16_t screenWidth = Consts::STANDARD_SCREEN_WIDTH, uint16_t screenHeight = Consts::STANDARD_SCREEN_HEIGHT,
                const std::string &name = Consts::PROJECT_NAME, const Color& background = Consts::BACKGROUND_COLOR);

    void exit();
};


#endif //INC_3DZAVR_TDZAVR_H
