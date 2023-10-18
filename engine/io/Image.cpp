//
// Created by Ivan Ilin on 21/09/2023.
//

#include "Image.h"

Image::Image(uint16_t width, uint16_t height) : _width(width), _height(height) {
    _arr = (uint8_t*)std::malloc(width * height * 4 * sizeof(uint8_t));
}

Image::CODE Image::save2png(const std::string &file_name, uint16_t bit_depth) {
    FILE * fp2 = fopen(file_name.c_str(), "wb");
    if (!fp2) {
        // dealing with error
        return FILE_OPEN_ERROR;
    }

    // 1. Create png struct pointer
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr){
        // dealing with error
        return PNG_STRUCT_ERROR;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        // dealing with error
        return INFO_STRUCT_ERROR;
    }

    // 2. Set png info like width, height, bit depth and color type
    //    in this example, I assumed grayscale image. You can change image type easily
    png_init_io(png_ptr, fp2);
    png_set_IHDR(png_ptr, info_ptr, _width, _height, bit_depth, \
    PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, \
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // 3. Convert 1d array to 2d array to be suitable for png struct
    //    I assumed the original array is 1d
    auto row_pointers = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytepp) * _height);
    for (int i = 0; i < _height; i++) {
        row_pointers[i] = (png_bytep)png_malloc(png_ptr, _width*4);
    }

    for (int hi = 0; hi < _height; hi++) {
        for (int wi = 0; wi < _width; wi++) {
            row_pointers[hi][wi*4 + 0] = _arr[4*(wi + _width * hi) + 0];
            row_pointers[hi][wi*4 + 1] = _arr[4*(wi + _width * hi) + 1];
            row_pointers[hi][wi*4 + 2] = _arr[4*(wi + _width * hi) + 2];
            row_pointers[hi][wi*4 + 3] = _arr[4*(wi + _width * hi) + 3];
        }
    }

    // 4. Write png file
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return SUCCESS;
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

Color Image::get_pixel(uint16_t x, uint16_t y) const {
    return Color(_arr[4*(x + _width*y) + 0], _arr[4*(x + _width*y) + 1], _arr[4*(x + _width*y) + 2], _arr[4*(x + _width*y) + 3]);
}
