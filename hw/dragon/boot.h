#ifndef HW_DRAGON_BOOT_H
#define HW_DRAGON_BOOT_H

#include "hw/boards.h"
#include "system/memory.h"
#include "dragon.h"

bool dragon_load_firmware(MemoryRegion *mr, const char* firmware);

#endif // HW_DRAGON_BOOT_H