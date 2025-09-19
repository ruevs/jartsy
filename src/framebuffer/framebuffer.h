#ifndef JARTSY_FRAMEBUFFER_H
#define JARTSY_FRAMEBUFFER_H

template<class T>
class FrameBuffer {
public:
    const unsigned rx, ry;
    T **buf;

    FrameBuffer(unsigned x, unsigned y) : rx(x), ry(y) {
        buf = new T*[rx];
        for(unsigned i = 0; i < rx; ++i) {
            buf[i] = new T[ry];
        }
    }

    ~FrameBuffer(){
        for(unsigned i = 0; i < rx; ++i) {
            delete buf[i];
        }
        delete buf;
    }

    inline T *operator[](unsigned x) {
        return buf[x];
    }
};

class RGBColor {
public:
    uint8_t red, green, blue;
};

#endif
