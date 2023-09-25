//
// Created by Ivan Ilin on 21/09/2023.
//

#include "Image.h"

Image::Image(uint16_t width, uint16_t height) : _width(width), _height(height) {
    _arr = (uint8_t*)std::malloc(width * height * 4 * sizeof(uint8_t));
}

LibPNG::CODE Image::save2png(const std::string &file_name, uint16_t bit_depth) {
    return LibPNG::arr2png(_arr, file_name, _width, _height, bit_depth);
}

Image::~Image() {
    // deallocate memory
    free(_arr);
}

void Image::set_pixel(uint16_t x, uint16_t y, const Color& color) {
    _arr[4*(x + _width*y) + 0] = color.r();
    _arr[4*(x + _width*y) + 1] = color.g();
    _arr[4*(x + _width*y) + 2] = color.b();
    _arr[4*(x + _width*y) + 3] = color.a();
}
