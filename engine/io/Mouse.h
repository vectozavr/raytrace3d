//
// Created by Иван Ильин on 19.09.2021.
//

#ifndef SHOOTER_MOUSE_H
#define SHOOTER_MOUSE_H

#include <memory>
#include <utility>

#include <io/Screen.h>
#include <linalg/Vec2D.h>

class Mouse final {
private:
    const std::shared_ptr<Screen> _screen;

    std::map<uint8_t, double> _tappedButtons;
public:
    explicit Mouse(std::shared_ptr<Screen> screen) : _screen(std::move(screen)) {};

    // returns true if this _button is _pressed
    static bool isButtonPressed(uint8_t button);

    // returns true if this _button is tapped and 1/5 sec passed (_button bouncing problem solved)
    bool isButtonTapped(uint8_t button);

    [[nodiscard]] Vec2D getMousePosition() const;
    [[nodiscard]] Vec2D getMouseDisplacement() const;

    void setMouseInCenter() const;
};


#endif //SHOOTER_MOUSE_H
