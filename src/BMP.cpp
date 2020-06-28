#include <inttypes.h>

#include "BMP.h"
#include "FileSystem.h"

#pragma pack (push, 1)
struct Header {
    char type[2];
    uint32_t size;
    uint16_t reserved[2];
    uint32_t offset;
};

struct DIB {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t depth;
};
#pragma pack (pop)

void loadBMP(uint8_t*& data, const char* path, uint32_t& x, uint32_t& y) {
    FILE* file = NULL;
    fopen_s(&file, path, "r");
    Header header;
    readStruct(file, header);
    DIB dib;
    readStruct(file, dib);
    seek(file, header.offset);
}
