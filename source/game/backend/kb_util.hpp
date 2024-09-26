#pragma once
#include "types.hpp"

int bitSize(u64 pNum)
{
    auto size = 0;
    while (pNum != 0)
    {
        pNum >>= 1;
        size++;
    }
    return size;
}