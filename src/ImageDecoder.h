#pragma once

#include <inttypes.h>

#pragma pack (push, 1)
struct Header {
    uint32_t width;
    uint32_t height;
};
struct Image {
    Header header;
    uint8_t* data;
};
#pragma pack (pop)

Image loadImage(const char* path);
