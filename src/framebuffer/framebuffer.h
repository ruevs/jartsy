#ifndef JARTSY_FRAMEBUFFER_H
#define JARTSY_FRAMEBUFFER_H

template<class T>
class FrameBuffer {
public:
    const size_t rx, ry;
    T **buf;

    FrameBuffer(size_t x, size_t y) : rx(x), ry(y) {
        buf = new T*[rx];
        for(int i = 0; i < rx; ++i) {
            buf[i] = new T[ry];
        }
    }

    ~FrameBuffer(){
        for(int i = 0; i < rx; ++i) {
            delete buf[i];
        }
        delete buf;
    }

    inline T *operator[](size_t x) {
        return buf[x];
    }
};

class RGBColor {
public:
    uint8_t red, green, blue;
};

#endif
