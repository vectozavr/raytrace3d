//
// Created by Иван Ильин on 14.01.2021.
//

#ifndef ENGINE_SCREEN_H
#define ENGINE_SCREEN_H


#include <string>
#include <map>

#include "SDL.h"

#include "objects/geometry/Triangle.h"
#include <utils/Time.h>
#include <Consts.h>
#include "objects/geometry/Mesh.h"
#include "objects/Camera.h"

class Screen final {
private:
    SDL_Renderer* _renderer;
    SDL_Window* _window;

    int _scene = 0;
    //std::vector<sf::Texture> _renderSequence;
    bool _renderVideo = false;

    std::string _title;

    Color _background;

    uint16_t _width;
    uint16_t _height;

    bool _isOpen = false;

public:
    void open(uint16_t screenWidth = Consts::STANDARD_SCREEN_WIDTH, uint16_t screenHeight = Consts::STANDARD_SCREEN_HEIGHT,
              const std::string &name = Consts::PROJECT_NAME, const Color& background = Consts::BACKGROUND_COLOR);

    void display();

    void clear();

    void drawTriangle(const Triangle &triangle);
    void drawLine(const Vec2D& from, const Vec2D& to);
    void drawPixel(const uint16_t x, const uint16_t y, const Color& color);

    void setTitle(const std::string &title);

    [[nodiscard]] std::string title() const { return _title; };

    [[nodiscard]] bool isOpen() const;

    [[nodiscard]] uint16_t width() const { return _width; }
    [[nodiscard]] uint16_t height() const { return _height; }

    void close();

    void startRender();
    void stopRender();
};


#endif //INC_3DZAVR_SCREEN_H
