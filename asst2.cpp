#include <iostream>
#include <cmath>

#include "SDL.h"

#include <Engine.h>
#include <io/Screen.h>
#include "objects/geometry/Sphere.h"
#include <utils/ObjectController.h>


class Test final : public Engine {
private:
    std::shared_ptr<ObjectController> cameraController = nullptr;
    std::shared_ptr<ObjectController> objController = nullptr;
    std::shared_ptr<Object> selectedObject = nullptr;
    bool objSelected = false;

    void start() override {
        std::shared_ptr<Sphere> sphere_1 = std::make_shared<Sphere>(0.7,
                                                                  Vec3D(-1.5, -0.3, 4),
                                                                  ObjectNameTag("sphere_1"),
                                                                  Consts::WHITE_COLORS[0]);
        world->add(sphere_1);

        std::shared_ptr<Sphere> sphere_2 = std::make_shared<Sphere>(1,
                                                                    Vec3D(0.5, 0, 3),
                                                                    ObjectNameTag("sphere_2"),
                                                                    Consts::WHITE_COLORS[2]);
        world->add(sphere_2);

        std::shared_ptr<Plane> plane = std::make_shared<Plane>(Vec3D(0, 1, 0),
                                                               Vec3D(0,-1,0),
                                                               ObjectNameTag("plane"),
                                                               Consts::WHITE*0.5);

        world->add(plane);

        std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>(ObjectNameTag("Directional Light Source 1"),
                                                                                     Vec3D(1, 1, -1),
                                                                                     Consts::WHITE);

        world->add(light);

        cameraController = std::make_shared<ObjectController>(camera);

    };
    void update() override {
        if(objSelected) {
            objController->update();
        } else {
            cameraController->update();
        }

        // select object:
        if (Keyboard::isKeyTapped(SDLK_o)) {
            auto rayCast = world->rayCast(camera->position(), camera->position() + camera->lookAt(), "plane");

            if(rayCast.intersected) {
                objSelected = true;
                selectedObject = rayCast.obj;
                objController = std::make_shared<ObjectController>(selectedObject);
                Log::log("Object " + rayCast.objectName.str() + " selected.");
            }
        }

        // deselect object:
        if (Keyboard::isKeyTapped(SDLK_p)) {
            objSelected = false;
        }

        // object scale x:
        if(Keyboard::isKeyTapped(SDLK_UP)) {
            selectedObject->scale(Vec3D(1 + Time::deltaTime(), 1, 1));
        }
        // object scale y:
        if(Keyboard::isKeyTapped(SDLK_DOWN)) {
            selectedObject->scale(Vec3D(1, 1 + Time::deltaTime(), 1));
        }
        // object scale z:
        if(Keyboard::isKeyTapped(SDLK_LEFT)) {
            selectedObject->scale(Vec3D(1, 1, 1 + Time::deltaTime()));
        }

        // undo transformations
        if(Keyboard::isKeyTapped(SDLK_u)) {
            selectedObject->transform(selectedObject->invModel());
        }

    };

public:
    Test() = default;
};


int main(int argc, char *argv[]) {
    Test test;
    test.create(300, 200);
}