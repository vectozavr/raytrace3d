//
// Created by Иван Ильин on 19.09.2021.
//

#include <io/Mouse.h>
#include <utils/Time.h>
#include <Consts.h>

Vec2D Mouse::getMousePosition() const {
    // TODO: implement
    return Vec2D(0, 0);
}

Vec2D Mouse::getMouseDisplacement() const {
    // TODO: implement
    setMouseInCenter();
    return Vec2D(0, 0);
}

void Mouse::setMouseInCenter() const {
    // TODO: implement
}

bool Mouse::isButtonPressed(uint8_t button) {
    // TODO: implement
    return false;
}

bool Mouse::isButtonTapped(uint8_t button) {
    if (!Mouse::isButtonPressed(button)) {
        return false;
    }

    if (_tappedButtons.count(button) == 0) {
        _tappedButtons.emplace(button, Time::time());
        return true;
    } else if ((Time::time() - _tappedButtons[button]) > Consts::TAP_DELAY) {
        _tappedButtons[button] = Time::time();
        return true;
    }
    return false;
}
