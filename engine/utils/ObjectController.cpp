//
// Created by Иван Ильин on 22.01.2022.
//

#include <utils/ObjectController.h>
#include <linalg/Vec2D.h>
#include <utils/Time.h>

ObjectController::ObjectController(std::shared_ptr<Object> object) : _object(std::move(object)){}

void ObjectController::update() {
    // Left and right
    if (Keyboard::isKeyPressed(SDLK_a))
        _object->translate(_object->left()*Time::deltaTime()*5.0);

    if (Keyboard::isKeyPressed(SDLK_d))
        _object->translate(-_object->left()*Time::deltaTime()*5.0);

    // Forward and backward
    if (Keyboard::isKeyPressed(SDLK_w))
        _object->translate(_object->lookAt()*Time::deltaTime()*5.0);

    if (Keyboard::isKeyPressed(SDLK_s))
        _object->translate(-_object->lookAt()*Time::deltaTime()*5.0);

    if (Keyboard::isKeyPressed(SDLK_LSHIFT))
        _object->translate(Vec3D{0.0, -Time::deltaTime()*5.0, 0});

    if (Keyboard::isKeyPressed(SDLK_SPACE))
        _object->translate(Vec3D{0.0, Time::deltaTime()*5.0, 0});

    // Mouse movement
    Vec2D mouseMotion = Mouse::getMouseDisplacement();

    _object->rotate(Vec3D{0, -mouseMotion.x()/1000.0, 0});
    _object->rotateLeft(mouseMotion.y()/1000.0);
}