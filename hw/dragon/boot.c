#include "qemu/osdep.h"
#include "qemu/datadir.h"
#include "hw/loader.h"
#include "qemu/error-report.h"
#include "boot.h"

bool dragon_load_firmware(MemoryRegion *mr, const char* firmware) {
    g_autofree char* filename = NULL;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, firmware);
    if (filename == NULL) {
        error_report("Failed to locate firmware image file: %s\n", firmware);
        return false;
    }

    int bytes_loaded = load_image_mr(filename, mr);
    if (bytes_loaded <= 0) {
        error_report("Failed to load firmware image file: %s\n", firmware);
        return false;
    }

    return true;
}