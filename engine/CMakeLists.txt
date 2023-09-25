cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

# create target
add_library(
        3DZAVR STATIC
        Consts.h
        Engine.h
        Engine.cpp
        World.h
        World.cpp
        Camera.h
        Camera.cpp
        Object.cpp
        Object.h
        linalg/Vec4D.h
        linalg/Vec4D.cpp
        linalg/Vec3D.cpp
        linalg/Vec3D.h
        linalg/Vec2D.cpp
        linalg/Vec2D.h
        linalg/Matrix4x4.h
        linalg/Matrix4x4.cpp
        linalg/Color.cpp
        linalg/Color.h
        geometry/Triangle.h
        geometry/Triangle.cpp
        geometry/Plane.h
        geometry/Plane.cpp
        geometry/Mesh.h
        geometry/Mesh.cpp
        physics/RigidBody.cpp
        physics/RigidBody.h
        physics/Simplex.h
        physics/HitBox.cpp
        physics/HitBox.h
        io/Screen.h
        io/Screen.cpp
        io/Keyboard.cpp
        io/Keyboard.h
        io/Mouse.cpp
        io/Mouse.h
        utils/Log.h
        utils/Log.cpp
        utils/Time.h
        utils/Time.cpp
        utils/Timer.cpp
        utils/Timer.h
        utils/LibPNG.cpp
        utils/LibPNG.h
        utils/ResourceManager.h
        utils/ResourceManager.cpp
        animation/Animation.h
        animation/Timeline.cpp
        animation/Timeline.h
        animation/Interpolation.h
        animation/Animation.cpp
        animation/ATranslate.h
        animation/AScale.h
        animation/ARotate.h
        animation/AWait.h
        animation/AFunction.h
        animation/AAttractToPoint.h
        animation/ARotateRelativePoint.h
        animation/ARotateLeft.h
        animation/Interpolation.cpp
        animation/Animations.h
        animation/AShowCreation.h
        animation/AShowUncreation.h
        animation/ARotateLeftUpLookAt.h
        animation/ADecompose.h io/Image.cpp io/Image.h)

target_include_directories(3DZAVR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

add_subdirectory(utils/libpng)
target_link_libraries(3DZAVR PUBLIC png)
target_include_directories(3DZAVR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/libpng)
