
//
// Created by Ivan Ilin on 21/09/2023.
//

#ifndef RAYTRACE3D_IMAGE_H
#define RAYTRACE3D_IMAGE_H


#include <cstdint>
#include <png.h>

#include <Consts.h>

class Image {
public:
    enum CODE {SUCCESS, FILE_OPEN_ERROR, PNG_STRUCT_ERROR, INFO_STRUCT_ERROR, ERROR};
private:
    const uint16_t _width;
    const uint16_t _height;

    uint8_t* _arr;
public:
    explicit Image(uint16_t width = Consts::STANDARD_SCREEN_WIDTH, uint16_t height = Consts::STANDARD_SCREEN_HEIGHT);

    [[nodiscard]] uint16_t width() const { return _width; }
    [[nodiscard]] uint16_t height() const { return _height; }

    void set_pixel(uint16_t x, uint16_t y, const Color& color);
    [[nodiscard]] Color get_pixel(uint16_t x, uint16_t y) const;

    CODE save2png(const std::string& file_name, uint16_t bit_depth = 8);

    ~Image();
};


#endif //RAYTRACE3D_IMAGE_H
