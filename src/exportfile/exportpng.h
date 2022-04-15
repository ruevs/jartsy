#ifndef JARTSY_EXPORTPNG_H
#define JARTSY_EXPORTPNG_H

#include "jartsy.h"
#include "platform/platform.h"
#include "framebuffer/framebuffer.h"

class PNGWriter {
public:
    static void ExportFrameBufferTo(const Platform::Path &filename, FrameBuffer<RGBColor> &fb);
};

#endif
