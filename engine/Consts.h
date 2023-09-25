//
// Created by Иван Ильин on 07.10.2021.
//

#ifndef SHOOTER_CONSTS_H
#define SHOOTER_CONSTS_H

#include <filesystem>

#include <linalg/Color.h>
#include <linalg/Vec2D.h>

namespace Consts {
    const uint16_t STANDARD_SCREEN_WIDTH = 1920;
    const uint16_t STANDARD_SCREEN_HEIGHT = 1080;
    const Color BACKGROUND_COLOR = Color(255, 255, 255);
    const std::string PROJECT_NAME = "engine";
    const bool USE_LOG_FILE = true;
    const bool SHOW_DEBUG_INFO = false;

    const double PI = 3.14159265358979323846264338327950288;
    const double EPS = 0.000001;

    const double EPA_EPS = 0.0001;

    const double RAY_CAST_MAX_DISTANCE = 10000;

    const double LARGEST_TIME_STEP = 1.0 / 15.0;
    const double TAP_DELAY = 0.2;

    const Vec2D BEZIER[2] = {Vec2D{0.8, 0}, Vec2D{0.2, 1}};

    const Color WHITE_COLORS[] = {
            Color(137, 135, 222), // blue
            Color(195, 155, 209), // pink
            Color(201, 137, 137), // red
            Color(116, 204, 135), // green
            Color(201, 171, 137), // orange
    };

    const Color DARK_COLORS[] = {
            Color(16, 18, 69), // blue
            Color(77, 0, 62), // pink
            Color(99, 20, 20), // red
            Color(12, 46, 9), // green
            Color(97, 70, 51), // orange
    };

    const Color WHITE = Color(255, 255, 255);
    const Color BLACK = Color(0, 0, 0);
}

#endif //SHOOTER_CONSTS_H
