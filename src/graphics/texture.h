#pragma once

#include "../utils/types.h"
#include <cstring>
#include <vector>

struct Texture
{
    auto setTextureData(u64 size, const u8* data) -> void
    {
        dataSize = size;
        pData.assign(data, data + size);
    }

    u32 width;
    u32 height;
    u32 depth;
    u32 mipLevels;
    std::vector<u64> mipOffset;
    u64 dataSize;
    std::vector<u8> pData;
};
