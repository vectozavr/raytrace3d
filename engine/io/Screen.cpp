//
// Created by Иван Ильин on 14.01.2021.
//

#include <utility>
#include <cmath>

#include "SDL.h"

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
    SDL_CreateWindowAndRenderer(_width*Consts::SCREEN_SCALE, _height*Consts::SCREEN_SCALE, 0, &_window, &_renderer);

    SDL_RenderSetScale(_renderer, Consts::SCREEN_SCALE, Consts::SCREEN_SCALE);

    SDL_SetRenderDrawColor(_renderer, background.r(), background.g(), background.b(), background.a());
    SDL_RenderClear(_renderer);

    SDL_ShowCursor(SDL_DISABLE);

    initDepthBuffer();
}

void Screen::initDepthBuffer() {
    _depthBuffer.clear();
    if(_height != 0 && _width != 0) {
        _depthBuffer.reserve(_width);
        for(uint16_t i = 0; i < _width; i++) {
            std::vector<float> column;
            column.reserve(_height);
            for(uint16_t j = 0; j < _height; j++) {
                column.push_back(1);
            }
            column.shrink_to_fit();
            _depthBuffer.push_back(column);
        }
        _depthBuffer.shrink_to_fit();
    }
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
        SDL_SetRenderDrawColor(_renderer, _background.r(), _background.g(), _background.b(), _background.a());
        SDL_RenderPresent(_renderer);
        SDL_WarpMouseInWindow(_window, (float)width()/2*Consts::SCREEN_SCALE, (float)height()/2*Consts::SCREEN_SCALE);
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
    if(x >= _width || x < 0 && y >= _height || y < 0)
        return;

    SDL_SetRenderDrawColor(_renderer, color.r(), color.g(), color.b(), color.a());
    SDL_RenderDrawPoint(_renderer, x, y);
}

void Screen::drawPixel(uint16_t x, uint16_t y, float z, const Color &color) {
    if(x >= _width || x < 0 && y >= _height || y < 0)
        return;

    if(z < _depthBuffer[x][y]) {
        drawPixel(x, y, color);
        _depthBuffer[x][y] = z;
    }
}

void Screen::drawLine(const Vec2D& from, const Vec2D& to, const Color &color, uint16_t thickness) {
    // TODO: It is better to use built in SDL functions
    //SDL_SetRenderDrawColor(_renderer, color.r(), color.g(), color.b(), color.a());
    //SDL_RenderLine(_renderer, (float)from.x(), (float)from.y(), (float)to.x(), (float)to.y());

    if(to.x() < from.x()) {
        return drawLine(to, from, color, thickness);
    }

    double der = (to.y() - from.y())/(to.x() - from.x());

    Vec2D from_to = (to - from).normalized();
    Vec2D norm(-from_to.y(), from_to.x());


    auto line = [from, to](double x_i, double y_i){
        return x_i*(to.y() - from.y()) + y_i*(from.x() - to.x()) + to.x()*from.y() - from.x()*to.y();
    };

    auto x = (int)from.x();
    auto y = (int)from.y();
    auto to_x = (int)to.x();
    auto to_y = (int)to.y();

    int step_y = to.y() - from.y() > 0 ? 1 : -1;
    int sign = std::abs(der) <= 1 ? 1 : -1;

    while (x < to_x || y*step_y < to_y*step_y) {
        for(int l = 0; l < thickness; l++) {
            drawPixel(x + norm.x()*l, y + norm.y()*l, color);
        }

        if(line(x, y)*step_y*sign > 0) {
            if(std::abs(der) <= 1) {
                y += step_y;
            } else {
                x += 1;
            }
        }
        if(std::abs(der) <= 1) {
            x += 1;
        } else {
            y += step_y;
        }
    }
}

void Screen::drawTriangle(const Triangle &triangle, std::shared_ptr<Texture> texture) {
    // Drawing edge
    //drawLine(Vec2D(triangle[0]), Vec2D(triangle[1]), triangle.colors()[0]);
    //drawLine(Vec2D(triangle[1]), Vec2D(triangle[2]), triangle.colors()[1]);
    //drawLine(Vec2D(triangle[2]), Vec2D(triangle[0]), triangle.colors()[2]);

    // Filling inside
    int x_min = (int)std::min({(double)_width, triangle[0].x(), triangle[1].x(), triangle[2].x()});
    int x_max = (int)std::max({0.0, triangle[0].x(), triangle[1].x(), triangle[2].x()}) + 1;
    int y_min = (int)std::min({(double)_height, triangle[0].y(), triangle[1].y(), triangle[2].y()});
    int y_max = (int)std::max({0.0, triangle[0].y(), triangle[1].y(), triangle[2].y()}) + 1;

    auto c = triangle.colors();
    auto tc = triangle.textureCoordinates();

    for(int y = y_min; y <= y_max; y++) {
        for(int x = x_min; x <= x_max; x++) {
            Vec3D abg = triangle.abgBarycCoord(Vec2D(x, y));

            if(abg.x() >= 0 && abg.y() >= 0 && abg.z() >= 0) {
                Color color;
                if(texture) {
                    Vec3D uv_hom = tc[0] + (tc[1] - tc[0])*abg.y() + (tc[2] - tc[0])*abg.z();
                    // De homogenite UV coordinates
                    Vec2D uv_dehom(uv_hom.x()/uv_hom.z(), uv_hom.y()/uv_hom.z());

                    // TODO: move calculations of derivatives somewhere from here: it becomes messy
                    // Derivative in X
                    Vec3D abg_xp = triangle.abgBarycCoord(Vec2D(x+1, y));
                    Vec3D uv_hom_xp = tc[0] + (tc[1] - tc[0])*abg_xp.y() + (tc[2] - tc[0])*abg_xp.z();
                    Vec2D uv_dehom_xp(uv_hom_xp.x()/uv_hom_xp.z(), uv_hom_xp.y()/uv_hom_xp.z());

                    // Derivative in Y
                    Vec3D abg_yp = triangle.abgBarycCoord(Vec2D(x, y+1));
                    Vec3D uv_hom_yp = tc[0] + (tc[1] - tc[0])*abg_yp.y() + (tc[2] - tc[0])*abg_yp.z();
                    Vec2D uv_dehom_yp(uv_hom_yp.x()/uv_hom_yp.z(), uv_hom_yp.y()/uv_hom_yp.z());

                    // Derivative in XY combined [Jacobian]
                    Vec2D du(texture->width()*(uv_dehom_xp - uv_dehom).x(), texture->width()*(uv_dehom_yp - uv_dehom).x());
                    Vec2D dv(texture->height()*(uv_dehom_xp - uv_dehom).y(), texture->height()*(uv_dehom_yp - uv_dehom).y());

                    color = texture->get_pixel_from_UV(uv_dehom, du.abs()+dv.abs());
                } else {
                    color = c[0]*abg.x() + c[1]*abg.y() + c[2]*abg.z();
                }
                float z = triangle[0].z()*abg.x() + triangle[1].z()*abg.y() + triangle[2].z()*abg.z();

                drawPixel(x, y, z, color);
            }
        }
    }
}

void Screen::setTitle(const std::string &title) {
    _title = title;
    SDL_SetWindowTitle(_window, title.c_str());
}

bool Screen::isOpen() const {
    return _isOpen;
}

void Screen::close() {
    _isOpen = false;

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();

    _renderer = nullptr;
    _window = nullptr;
}

void Screen::clearDepthBuffer() {
    for (auto& row : _depthBuffer) {
        for(auto& el : row) {
            el = 1;
        }
    }
}

void Screen::drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) {
    drawTriangle(Triangle({Vec4D(x, y), Vec4D(x+width, y), Vec4D(x, y+height)},
                          {},
                          {color, color, color}));

    drawTriangle(Triangle({Vec4D(x, y+height), Vec4D(x+width, y), Vec4D(x+width, y+height)},
                          {},
                          {color, color, color}));
}

void Screen::drawStrokeRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color,
                                 uint16_t thickness, const Color &strokeColor) {

    drawRectangle(x, y, width, height, color);
    drawLine(Vec2D(x, y), Vec2D(x + width, y), strokeColor);
    drawLine(Vec2D(x+ width, y), Vec2D(x + width, y + height), strokeColor);
    drawLine(Vec2D(x + width, y + height), Vec2D(x, y + height), strokeColor);
    drawLine(Vec2D(x, y + height), Vec2D(x, y), strokeColor);
}

void Screen::drawImage(uint16_t x, uint16_t y, std::shared_ptr<Image> img) {
    for(int i = 0; i < img->width(); i++) {
        for(int j = 0; j < img->height(); j++) {
            drawPixel(i + x, j + y, img->get_pixel(i, j));
        }
    }
}
