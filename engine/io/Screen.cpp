//
// Created by Иван Ильин on 14.01.2021.
//

#include <utility>
#include <cmath>

#include <io/Screen.h>
#include <utils/Time.h>
#include <utils/Log.h>
#include <utils/ResourceManager.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define popen _popen
#endif

void Screen::open(uint16_t screenWidth, uint16_t screenHeight, const std::string &name, const Color& background) {
    _title = name;
    _background = background;
    _width = screenWidth;
    _height = screenHeight;

    // TODO: implement
}

void Screen::display() {

    std::string title = _title + " (" + std::to_string(Time::fps()) + " fps)";

    if(_renderVideo) {
        // most of the time of video rendering is wasting on saving .png sequence
        // that's why we will save all images in the end

        //_renderSequence.push_back(screenState);
    }

}

void Screen::startRender() {
    stopRender();

    Log::log("Screen::startRender(): start recording the screen");
    _renderVideo = true;
}

void Screen::stopRender() {
    if(_renderVideo) {
        Log::log("Screen::stopRender(): stop recording the screen");
        Log::log("Screen::stopRender(): start saving .png sequence");
        std::string c = "rm film/png/*.png";
        popen(c.c_str(), "w");
        int i = 0;
        /*
        for(; i < _renderSequence.size(); i++) {
            _renderSequence[i].copyToImage().saveToFile("film/png/" + std::to_string(i) + ".png");
            Log::log("Screen::stopRender(): saving .png sequence (" + std::to_string(static_cast<int>(100*i/_renderSequence.size())) + "%)");
        }
        _renderSequence.clear();
        */
        Log::log("Screen::stopRender(): start rendering final video");
        // TODO: .png sequence looks better than a final video (poor clarity and desaturated colors)
        c = "ffmpeg -stats -r 60 -i film/png/%d.png -vcodec libx264 -crf 1 -pix_fmt yuv420p -frames " + std::to_string(i) + " film/mp4/" + std::to_string(_scene) + "_" + _title + "_" + std::to_string(rand()) + ".mp4";
        popen(c.c_str(), "w");
        _scene++;
        _renderVideo = false;
        Log::log("Screen::stopRender(): finish rendering final video");
    }
}

void Screen::clear() {
    // TODO: implement
}

void Screen::drawTriangle(const Triangle &triangle) {
    // TODO: implement
}

void Screen::setTitle(const std::string &title) {
    _title = title;
}

bool Screen::isOpen() {
    // TODO: implement
    return false;
}

void Screen::close() {
    // TODO: implement
}
