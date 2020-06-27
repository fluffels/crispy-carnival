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
#pragma pack (pop)

void loadBMP(uint8_t*& data, const char* path, uint32_t& x, uint32_t& y) {
    FILE* file = NULL;
    fopen_s(&file, path, "r");
    Header header;
    readStruct(file, header);
    seek(file, header.offset);
}
