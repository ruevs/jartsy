#include "exportpgm.h"
#include <cstdio>

void PGMWriter::ExportFrameBufferTo(const Platform::Path &filename, FrameBuffer<RGBColor> &fb) {
    FILE *f;

    f = OpenFile(filename, "wb");
    if(!f) {
        fprintf(stderr, "Couldn't write to '%s'", filename.raw.c_str());
        return;
    }

    const char *PGMHeader = "P3\n";
    fprintf(f, PGMHeader);
    fprintf(f, "%zu %zu\n", fb.rx, fb.ry);    // Resolution
    fprintf(f, "%u\n", std::numeric_limits<uint8_t>::max()); // sub-pixel color gepth
    for(size_t y = 0; y < fb.ry; ++y) {
        for(size_t x = 0; x < fb.rx; ++x) {
            fprintf(f, "%u\t%u\t%u\t", fb[x][y].red, fb[x][y].green, fb[x][y].blue); // One pixel
        }
        fprintf(f, "\n");
    }
}
