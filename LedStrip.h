#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <stdint.h>
#include <array>

typedef uint32_t NativeColor; // 0xWWRRGGBB

#define NATIVE_GET_WHITE(x) (((x) >> 24) & 0xff)
#define NATIVE_GET_RED(x) (((x) >> 16) & 0xff)
#define NATIVE_GET_GREEN(x) (((x) >> 8) & 0xff)
#define NATIVE_GET_BLUE(x) ((x) & 0xff)
#define NATIVE_SET_RGBW(r, g, b, w) ((b) | (((g) & 0xff) << 8) | (((r) & 0xff) << 16) | (((w) & 0xff) << 24))

class LedStrip {
public:
    enum {NUM_PIXELS = 131};
    virtual ~LedStrip()=0;

    // Setup and stop
    virtual void init()=0;
    virtual void term()=0;

    // Basic screen management
    /** Turn all LEDs off */
    virtual void reset()=0;
    /** Push colors to strip */
    virtual void refresh(const std::array<NativeColor, NUM_PIXELS>&& pixels)=0;
};

#endif // LEDSTRIP_H
