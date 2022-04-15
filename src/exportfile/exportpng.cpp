#include "exportpng.h"
#include "extlib/lodepng/lodepng.h"
#include "extlib/lodepng/lodepng.cpp"   // PAR@@@ Nasty!

void PNGWriter::ExportFrameBufferTo(const Platform::Path &filename, FrameBuffer<RGBColor> &fb) {
    std::vector<unsigned char> png;
    std::vector<uint8_t> image;

    for(size_t y = 0; y < fb.ry; ++y) {
        for(size_t x = 0; x < fb.rx; ++x) {
            image.push_back(fb.buf[x][y].red);
            image.push_back(fb.buf[x][y].green);
            image.push_back(fb.buf[x][y].blue);
            image.push_back(255); // Alpha
        }
    }

    unsigned error = lodepng::encode(png, image, fb.rx, fb.ry);
    if(!error) {
        error = lodepng::save_file(png, filename.raw);
    }

    if(error) {
        fprintf(stderr, "Couldn't write to '%s'\r\nPNG encoder error %u: %s\r\n",
                filename.FileName().c_str(), error, lodepng_error_text(error));
    }
}