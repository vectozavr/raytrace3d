//
// Created by Иван Ильин on 13.01.2021.
//

#include <sstream>
#include <cmath>

#include <World.h>
#include <utils/Log.h>
#include "objects/geometry/Plane.h"
#include <utils/ResourceManager.h>

#include <iostream>


std::shared_ptr<Object> World::add(std::shared_ptr<Object> object) {
    _objects.emplace(object->name(), object);
    // TODO: add different cases when the object either the Mesh or RigidBody
    //Log::log("World::addObject(): inserted object '" + object->name().str() + "' with " +
    //         std::to_string(_objects[object->name()]->triangles().size()) + " tris.");

    Log::log("World::add(): inserted object '" + object->name().str() + "'");

    return _objects[object->name()];
}

void World::remove(const ObjectNameTag &tag) {
    if (_objects.erase(tag) > 0) {
        Log::log("World::remove(): removed body '" + tag.str() + "'");
    } else {
        Log::log("World::remove(): cannot remove body '" + tag.str() + "': body does not exist.");
    }
}

std::shared_ptr<Mesh> World::loadMesh(const ObjectNameTag &tag, const std::string &filename, const Vec3D &scale) {
    std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(tag, filename, scale);

    _objects.emplace(tag, newMesh);
    Log::log("World::loadMesh(): inserted body from " + filename + " with title '" + tag.str() + "' with " +
             std::to_string(newMesh->triangles().size()) + " tris.");
    return newMesh;
}

Object::IntersectionInformation World::rayCast(const Vec3D &from, const Vec3D &to, const std::string &skipTags) {

    // make vector of tags, that we are going to escape
    std::vector<std::string> tagsToSkip;
    std::stringstream s(skipTags);
    std::string t;
    while (s >> t) {
        tagsToSkip.push_back(t);
    }

    bool intersected = false;
    Vec3D point{};
    Vec3D norm{};
    Triangle triangle;
    Color color;
    std::string bodyName;
    double minDistance = std::numeric_limits<double>::infinity();
    std::shared_ptr<Object> intersectedBody = nullptr;

    for (auto&[name, object]  : _objects) {

        bool escapeThisBody = false;
        for (auto &escapeTag : tagsToSkip) {
            if (name == ObjectNameTag(escapeTag)) {
                escapeThisBody = true;
                break;
            }
        }
        if (escapeThisBody) {
            continue;
        }

        auto intersection = object->intersect(from, to);

        if(intersection.intersected && intersection.distanceToObject < minDistance) {
            minDistance = intersection.distanceToObject;
            point = intersection.pointOfIntersection;
            bodyName = name.str();
            intersected = true;
            intersectedBody = object;
            norm = intersection.normal;
            color = intersection.color;

            //Triangle triangleRED = Triangle(model * tri[0], model * tri[1], model * tri[2], sf::Color(255, 0, 0));
            //addObject(std::make_shared<RigidBody>(Mesh(ObjectNameTag("Test" + std::to_string(rand())), std::vector<Triangle>({triangleRED}))));
        }

    }

    return Object::IntersectionInformation{point,
                                           norm,
                                           minDistance,
                                           ObjectNameTag(bodyName),
                                           intersectedBody,
                                           intersected,
                                           color};
}

void World::loadMap(const std::string &filename, const Vec3D &scale) {
    auto objs = ResourceManager::loadObjects(filename);
    for (auto &i : objs) {
        std::shared_ptr<RigidBody> obj = std::make_shared<RigidBody>(*i, false);
        add(obj);
        obj->scale(scale);
    }
}

void World::checkCollision(const ObjectNameTag &tag) {
    std::shared_ptr<RigidBody> rigidBodyObj = std::dynamic_pointer_cast<RigidBody>(_objects[tag]);
    if (!rigidBodyObj) {
        // The case when we cannot cast Object -> RigidBody
        return;
    }

    if (rigidBodyObj->hasCollision()) {

        rigidBodyObj->setInCollision(false);

        for (auto it = _objects.begin(); it != _objects.end();) {

            std::shared_ptr<RigidBody> obj = std::dynamic_pointer_cast<RigidBody>(it->second);
            if (!obj) {
                // The case when we cannot cast Object -> RigidBody
                continue;
            }

            ObjectNameTag name = it->first;
            it++;

            if ((name == tag) || !(obj->isCollider() || obj->isTrigger())) {
                continue;
            }

            std::pair<bool, Simplex> gjk = rigidBodyObj->checkGJKCollision(obj);
            if (gjk.first) {
                if (obj->isCollider()) {
                    CollisionPoint epa = rigidBodyObj->EPA(gjk.second, obj);
                    rigidBodyObj->solveCollision(epa);
                }
                if (rigidBodyObj->collisionCallBack() != nullptr) {
                    rigidBodyObj->collisionCallBack()(name, obj);
                }
            }

        }
    }
}

void World::update() {
    for (auto &[nameTag, obj] : _objects) {
        std::shared_ptr<RigidBody> rigidBodyObj = std::dynamic_pointer_cast<RigidBody>(obj);
        if(rigidBodyObj) {
            rigidBodyObj->updatePhysicsState();
            checkCollision(nameTag);
        }
    }
}

std::shared_ptr<Object> World::object(const ObjectNameTag &tag) {
    if (_objects.count(tag) == 0) {
        return nullptr;
    }
    return _objects.find(tag)->second;
}

std::shared_ptr<Object> World::add(std::shared_ptr<DirectionalLight> light) {
    _lightSources.emplace(light->name(), light);

    add(std::dynamic_pointer_cast<Object>(light));

    return _objects[light->name()];
}

Color World::getIllumination(const Object::IntersectionInformation &point,
                             const Vec3D& cameraPosition,
                             const Vec3D& cameraDirection) {

    Color result;

    Vec3D norm = point.normal;
    Color surfaceColor = point.color;
    Vec3D d = cameraDirection;
    Vec3D v = -cameraDirection;
    Vec3D e = cameraPosition;

    // Constant minimal illumination from the environment
    Color env = point.color*0.3;

    result = result + env;

    for (auto &[nameTag, light] : _lightSources) {
        // Light source:
        Color lightColor = light->color();
        Vec3D lightDir = light->direction();

        // Lambertian Illumination
        Color lamb = lightColor*surfaceColor*std::max(0.0, lightDir.dot(norm))*0.7;

        // Blinn-Phong Illumination
        double p = 20;
        Vec3D h = (lightDir+v).normalized();
        double dot = h.dot(norm);
        if(dot < 0)
            dot = 0;

        Color spec = Consts::WHITE*lightColor*std::pow(dot, p)*0.5;

        // Shadows
        auto rc_shadow = rayCast(point.pointOfIntersection,
                                 point.pointOfIntersection + lightDir,
                                 point.objectName.str());
        if(rc_shadow.distanceToObject < std::numeric_limits<double>::infinity()) {
            lamb = lamb*0;
            spec = spec*0;
        }

        result = result + lamb + spec;

        // TODO: Reflections
        /*
        if(t < std::numeric_limits<double>::infinity() && flag != REFLECTION) {
            // Reflections
            Vec3D r = norm*2*v.dot(norm) - v;
            RayCastInfo rc_reflection = rayCast(e + d*t, r, REFLECTION);

            env = env + rc_reflection.color*0.5/(1.0 + rc_reflection.distance/5.0);
        }
        */
    }

    // Fade with distance
    //return RayCastInfo{d*t, norm, (env + lamb1 + spec1 + lamb2 + spec2)/(1.0 + d.abs()*t/30.0), d.abs()*t};
    //result /= 1.0 + d.abs()*t/30.0;

    return result;
}


