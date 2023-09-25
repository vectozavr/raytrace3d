//
// Created by Иван Ильин on 19.09.2021.
//

#include <io/Keyboard.h>
#include <utils/Time.h>
#include <Consts.h>

bool Keyboard::isKeyPressed(uint8_t key) {
    // TODO: implement

    return false;
}


bool Keyboard::isKeyTapped(uint8_t key) {
    if (!Keyboard::isKeyPressed(key)) {
        return false;
    }

    if (_tappedKeys.count(key) == 0) {
        _tappedKeys.emplace(key, Time::time());
        return true;
    } else if ((Time::time() - _tappedKeys[key]) > Consts::TAP_DELAY) {
        _tappedKeys[key] = Time::time();
        return true;
    }
    return false;
}

