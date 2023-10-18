//
// Created by Иван Ильин on 19.09.2021.
//


#include <io/Keyboard.h>
#include <utils/Time.h>
#include <Consts.h>
#include <utils/Log.h>

Keyboard *Keyboard::_instance = nullptr;

void Keyboard::init() {
    delete _instance;
    _instance = new Keyboard();

    Log::log("Keyboard::init(): keyboard was initialized");
}

bool Keyboard::isKeyPressed(SDL_Keycode key) {

    if (_instance == nullptr) {
        return false;
    }

    if(_instance->_keys.find(key)->second) {
        return _instance->_keys[key];
    }

    return false;
}

bool Keyboard::isKeyTapped(SDL_Keycode key) {
    if (!Keyboard::isKeyPressed(key)) {
        return false;
    }

    if (_instance->_tappedKeys.count(key) == 0) {
        _instance->_tappedKeys.emplace(key, Time::time());
        return true;
    } else if ((Time::time() - _instance->_tappedKeys[key]) > Consts::TAP_DELAY) {
        _instance->_tappedKeys[key] = Time::time();
        return true;
    }
    return false;
}

void Keyboard::sendKeyboardEvent(const SDL_Event &event) {
    switch (event.type) {
        // exit if the window is closed
        case SDL_EVENT_KEY_DOWN:
            _instance->_keys[event.key.keysym.sym] = true;
            break;
        case SDL_EVENT_KEY_UP:
            _instance->_keys[event.key.keysym.sym] = false;
            break;
    }
}

void Keyboard::free() {
    delete _instance;
    _instance = nullptr;

    Log::log("Keyboard::free(): keyboard was freed");
}
