//
// Created by Иван Ильин on 19.09.2021.
//

#ifndef SHOOTER_KEYBOARD_H
#define SHOOTER_KEYBOARD_H

#include <map>
#include <cstdint>

class Keyboard final {
private:
    std::map<uint8_t, double> _tappedKeys;
public:
    Keyboard() = default;

    // returns true if this key is _pressed
    static bool isKeyPressed(uint8_t key);

    // returns true if this key is tapped and 1/5 sec passed (_button bouncing problem solved)
    bool isKeyTapped(uint8_t key);
};


#endif //SHOOTER_KEYBOARD_H
