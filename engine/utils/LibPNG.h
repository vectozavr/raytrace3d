//
// Created by Ivan Ilin on 20/09/2023.
//

#ifndef RAYTRACE3D_LIBPNG_H
#define RAYTRACE3D_LIBPNG_H

#include <cstdint>
#include <string>

#include <png.h>

class LibPNG final {
public:
    enum CODE {SUCCESS, FILE_OPEN_ERROR, PNG_STRUCT_ERROR, INFO_STRUCT_ERROR, ERROR};

    static CODE arr2png(const uint8_t* bmp_source, const std::string& file_name, uint16_t width, uint16_t height, uint16_t bit_depth = 8);
};


#endif //RAYTRACE3D_LIBPNG_H
