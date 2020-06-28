#include <inttypes.h>

#include "ImageDecoder.h"
#include "FileSystem.h"

Image loadImage(const char* path) {
    Image result = {};

    FILE* file = NULL;
    fopen_s(&file, path, "rb");

    readStruct(file, result.header);
    auto size = result.header.width * result.header.height * 4;

    result.data = new uint8_t[size];
    auto count = fread(result.data, size, 1, file);

    return result;
}
