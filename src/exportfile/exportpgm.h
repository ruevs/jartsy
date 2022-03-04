#ifndef JARTSY_EXPORTPGM_H
#define JARTSY_EXPORTPGM_H

#include "jartsy.h"
#include "platform/platform.h"
#include "framebuffer/framebuffer.h"

class PGMWriter {
public:
    static void ExportFrameBufferTo(const Platform::Path &filename, FrameBuffer<RGBColor> &fb);
};

#endif
