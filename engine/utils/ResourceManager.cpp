//
// Created by Neirokan on 09.05.2020
//

#include <sstream>
#include <fstream>
#include <memory>
#include <map>

#include <utils/ResourceManager.h>
#include <utils/Log.h>

ResourceManager *ResourceManager::_instance = nullptr;

void ResourceManager::init() {
    delete _instance;
    _instance = new ResourceManager();

    Log::log("ResourceManager::init(): resource manager was initialized");
}

std::vector<std::shared_ptr<Mesh>> ResourceManager::loadObjects(const std::string &filename) {

    std::vector<std::shared_ptr<Mesh>> objects{};
    std::map<std::string, Color> maters{};

    if (_instance == nullptr) {
        return objects;
    }


    // If objects is already loaded - return pointer to it
    auto it = _instance->_objects.find(filename);
    if (it != _instance->_objects.end()) {
        return it->second;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        Log::log("Mesh::LoadObjects(): cannot load file from '" + filename + "'");
        return objects;
    }

    std::vector<Vec4D> verts{};
    std::vector<Triangle> tris{};
    Color currentColor = Color(255, 245, 194, 255);

    while (!file.eof()) {
        char line[128];
        file.getline(line, 128);

        std::stringstream s;
        s << line;

        char junk;
        if (line[0] == 'o') {
            if (!tris.empty())
                objects.push_back(
                        std::make_shared<Mesh>(ObjectNameTag(filename + "_temp_obj_" + std::to_string(objects.size())), tris));
            tris.clear();
        }
        if (line[0] == 'v') {
            double x, y, z;
            s >> junk >> x >> y >> z;
            verts.emplace_back(x, y, z, 1.0);
        }
        if (line[0] == 'g') {
            std::string matInfo;
            s >> junk >> matInfo;
            std::string colorName = matInfo.substr(matInfo.size() - 3, 3);
            currentColor = maters[matInfo.substr(matInfo.size() - 3, 3)];
        }
        if (line[0] == 'f') {
            int f[3];
            s >> junk >> f[0] >> f[1] >> f[2];
            tris.emplace_back(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1], currentColor);
        }
        if (line[0] == 'm') {
            int color[4];
            std::string matName;

            s >> junk >> matName >> color[0] >> color[1] >> color[2] >> color[3];
            maters.insert({matName, Color(color[0], color[1], color[2], color[3])});
        }
    }

    if (!tris.empty()) {
        objects.push_back(
                std::make_shared<Mesh>(ObjectNameTag(filename + "_temp_obj_" + std::to_string(objects.size())), tris));
    }
    tris.clear();

    file.close();

    Log::log("Mesh::LoadObjects(): obj '" + filename + "' was loaded");

    // If success - remember and return vector of objects pointer
    _instance->_objects.emplace(filename, objects);

    return objects;
}

void ResourceManager::unloadObjects() {
    if (_instance == nullptr) {
        return;
    }

    int objCounter = _instance->_objects.size();
    _instance->_objects.clear();

    Log::log("ResourceManager::unloadObjects(): all " + std::to_string(objCounter) + " objects was unloaded");
}

void ResourceManager::unloadAllResources() {
    unloadObjects();

    Log::log("ResourceManager::unloadAllResources(): all resources was unloaded");
}

void ResourceManager::free() {
    unloadAllResources();

    delete _instance;
    _instance = nullptr;

    Log::log("ResourceManager::free(): pointer to 'ResourceManager' was freed");
}
