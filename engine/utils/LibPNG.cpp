//
// Created by Ivan Ilin on 20/09/2023.
//

#include "LibPNG.h"

LibPNG::CODE
LibPNG::arr2png(const uint8_t *bmp_source, const std::string& file_name, uint16_t width, uint16_t height, uint16_t bit_depth) {

    FILE * fp2 = fopen(file_name.c_str(), "wb");
    if (!fp2) {
        // dealing with error
        return LibPNG::FILE_OPEN_ERROR;
    }

    // 1. Create png struct pointer
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr){
        // dealing with error
        return LibPNG::PNG_STRUCT_ERROR;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        // dealing with error
        return LibPNG::INFO_STRUCT_ERROR;
    }

    // 2. Set png info like width, height, bit depth and color type
    //    in this example, I assumed grayscale image. You can change image type easily
    png_init_io(png_ptr, fp2);
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, \
    PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, \
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // 3. Convert 1d array to 2d array to be suitable for png struct
    //    I assumed the original array is 1d
    auto row_pointers = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytepp) * height);
    for (int i = 0; i < height; i++) {
        row_pointers[i] = (png_bytep)png_malloc(png_ptr, width*4);
    }

    for (int hi = 0; hi < height; hi++) {
        for (int wi = 0; wi < width; wi++) {
            row_pointers[hi][wi*4 + 0] = bmp_source[4*(wi + width * hi) + 0];
            row_pointers[hi][wi*4 + 1] = bmp_source[4*(wi + width * hi) + 1];
            row_pointers[hi][wi*4 + 2] = bmp_source[4*(wi + width * hi) + 2];
            row_pointers[hi][wi*4 + 3] = bmp_source[4*(wi + width * hi) + 3];
        }
    }

    // 4. Write png file
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return LibPNG::SUCCESS;
}
