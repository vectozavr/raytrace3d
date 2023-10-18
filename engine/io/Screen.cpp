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

    _isOpen = true;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(_width, _height, 0, &_window, &_renderer);
    SDL_SetRenderScale(_renderer, 1, 1);

    SDL_SetRenderDrawColor(_renderer, background.r(), background.g(), background.b(), background.a());
    SDL_RenderClear(_renderer);

    SDL_HideCursor();
}

void Screen::display() {

    std::string title = _title + " (" + std::to_string(Time::fps()) + " fps)";

    if(_renderVideo) {
        // most of the time of video rendering is wasting on saving .png sequence
        // that's why we will save all images in the end

        // TODO: implement saving screen state into _renderSequence
        //_renderSequence.push_back(screenState);
    }

    if(_isOpen) {
        SDL_RenderPresent(_renderer);

        SDL_WarpMouseInWindow(_window, (float)width()/2, (float)height()/2);
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
    SDL_RenderClear(_renderer);
}

void Screen::drawPixel(const uint16_t x, const uint16_t y, const Color &color) {
    SDL_SetRenderDrawColor(_renderer, color.r(), color.g(), color.b(), color.a());
    SDL_RenderPoint(_renderer, x, y);
}

void Screen::drawTriangle(const Triangle &triangle) {
    // TODO: implement
}

void Screen::drawLine(const Vec2D& from, const Vec2D& to) {
    // TODO: implement
}

void Screen::setTitle(const std::string &title) {
    _title = title;
}

bool Screen::isOpen() const {
    return _isOpen;
}

void Screen::close() {
    _isOpen = false;

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}
