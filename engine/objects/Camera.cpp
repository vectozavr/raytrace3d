//
// Created by Иван Ильин on 14.01.2021.
//

#include <cmath>

#include "utils/Log.h"
#include "Camera.h"
#include "Consts.h"

std::vector<std::pair<std::shared_ptr<Triangle>, std::shared_ptr<Texture>>> Camera::project(std::shared_ptr<Mesh> mesh) {

    if (!_ready) {
        Log::log("Camera::project(): cannot project _tris without camera initialization ( Camera::init() ) ");
        return _triangles;
    }

    if (!mesh->isVisible()) {
        return this->_triangles;
    }

    // Model transform matrix: translate _tris in the origin of body.
    Matrix4x4 M = mesh->model();
    Matrix4x4 V = invModel();

    // We don't want to waste time re-allocating memory every time
    std::vector<Triangle> clippedTriangles, tempBuffer;

    for (auto &t : mesh->triangles()) {

        Triangle MTriangle = t * M;

        double dot = MTriangle.norm().dot((Vec3D(MTriangle.position()) - position()).normalized());

        if (dot > 0) {
            continue;
        }

        Triangle VMTriangle = MTriangle * V;

        // It needs to be cleared because it's reused through iterations. Usually it doesn't free memory.
        clippedTriangles.clear();
        tempBuffer.clear();

        // In the beginning we need to translate triangle from world coordinate to our camera system:
        // After that we apply clipping for all planes from _clipPlanes
        clippedTriangles.emplace_back(VMTriangle);
        for (auto &plane : _clipPlanes) {
            while (!clippedTriangles.empty()) {
                std::vector<Triangle> clipResult = plane.clip(clippedTriangles.back());
                clippedTriangles.pop_back();
                for (auto &i : clipResult) {
                    tempBuffer.emplace_back(i);
                }
            }
            clippedTriangles.swap(tempBuffer);
        }

        for (auto &clipped : clippedTriangles) {
            auto colors = clipped.colors();
            auto ambientColors = std::array<Color, 3>{colors[0]*(0.3 * std::abs(dot) + 0.7),
                                                      colors[1]*(0.3 * std::abs(dot) + 0.7),
                                                      colors[2]*(0.3 * std::abs(dot) + 0.7)};

            // Finally its time to project our clipped colored drawTriangle from 3D -> 2D
            // and transform it's coordinate to screen space (in pixels):
            Triangle clippedProjected = clipped * _SP;
            auto clippedTexCoord = clippedProjected.textureCoordinates();

            Triangle clippedProjectedNormalized = Triangle({clippedProjected[0] / clippedProjected[0].w(),
                                                            clippedProjected[1] / clippedProjected[1].w(),
                                                            clippedProjected[2] / clippedProjected[2].w()},
                                                           {clippedTexCoord[0] / clippedProjected[0].w(),
                                                            clippedTexCoord[1] / clippedProjected[1].w(),
                                                            clippedTexCoord[2] / clippedProjected[2].w()},
                                                           ambientColors);

            auto textureRef = mesh->getTexture();
            auto tri = std::make_shared<Triangle>(clippedProjectedNormalized);
            std::pair<std::shared_ptr<Triangle>, std::shared_ptr<Texture>> pair(tri, textureRef);

            _triangles.emplace_back(pair);
        }
    }

    return this->_triangles;
}

void Camera::init(int width, int height, double fov, double ZNear, double ZFar) {
    // We need to init camera only after creation or changing width, height, fov, ZNear or ZFar.
    // Because here we calculate matrix that does not change during the motion of _objects or camera
    _aspect = (double) width / (double) height;
    Matrix4x4 P = Matrix4x4::Projection(fov, _aspect, ZNear, ZFar);
    Matrix4x4 S = Matrix4x4::ScreenSpace(width, height);

    _SP = S * P; // screen-space-projections matrix

    // This is planes for clipping _tris.
    // Motivation: we are not interested in _tris that we cannot see.
    _clipPlanes.emplace_back(Vec3D{0, 0, 1}, Vec3D{0, 0, ZNear}, ObjectNameTag("near")); // near plane
    _clipPlanes.emplace_back(Vec3D{0, 0, -1}, Vec3D{0, 0, ZFar}, ObjectNameTag("far")); // far plane

    double thetta1 = Consts::PI * fov * 0.5 / 180.0;
    double thetta2 = atan(_aspect * tan(thetta1));
    _clipPlanes.emplace_back(Vec3D{-cos(thetta2), 0, sin(thetta2)}, Vec3D{0, 0, 0}, ObjectNameTag("left")); // left plane
    _clipPlanes.emplace_back(Vec3D{cos(thetta2), 0, sin(thetta2)}, Vec3D{0, 0, 0}, ObjectNameTag("right")); // right plane
    _clipPlanes.emplace_back(Vec3D{0, cos(thetta1), sin(thetta1)}, Vec3D{0, 0, 0}, ObjectNameTag("down")); // down plane
    _clipPlanes.emplace_back(Vec3D{0, -cos(thetta1), sin(thetta1)}, Vec3D{0, 0, 0}, ObjectNameTag("up")); // up plane

    _ready = true;
    Log::log("Camera::init(): camera successfully initialized.");
}

std::vector<std::pair<std::shared_ptr<Triangle>, std::shared_ptr<Texture>>> Camera::sorted() {

    // Sort _tris from _back to front
    // This is some replacement for Z-buffer
    std::sort(_triangles.begin(), _triangles.end(), [](auto &t1, auto &t2) {
        std::vector<double> v_z1({(*t1.first)[0].z(), (*t1.first)[1].z(), (*t1.first)[2].z()});
        std::vector<double> v_z2({(*t2.first)[0].z(), (*t2.first)[1].z(), (*t2.first)[2].z()});

        std::sort(v_z1.begin(), v_z1.end());
        std::sort(v_z2.begin(), v_z2.end());

        double z1 = v_z1[0] + v_z1[1] + v_z1[2];
        double z2 = v_z2[0] + v_z2[1] + v_z2[2];

        return z1 > z2;
    });

    return _triangles;
}

void Camera::clear() {
    // Cleaning all _tris and recalculation of View matrix
    _triangles.clear();
}
