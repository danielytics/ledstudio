#include "Color.h"

#include "LedStrip.h"

float hueToRGB(float v1, float v2, float vH) {
    if (vH < 0) vH += 1;
    if (vH > 1) vH -= 1;
    if ((6 * vH) < 1) return (v1 + (v2 - v1) * 6 * vH);
    if ((2 * vH) < 1) return v2;
    if ((3 * vH) < 2) return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);
    return v1;
}

unsigned Color::toRGBW(const Color& color)
{
    unsigned r = 0;
    unsigned g = 0;
    unsigned b = 0;
    if (color.saturation <= 0)
    {
        r = g = b = unsigned(color.lightness * 255);
    } else {
        float hue = color.hue / 360.0f;
        float v2 = (color.lightness < 0.5f) ? (color.lightness * (1 + color.saturation)) : ((color.lightness + color.saturation) - (color.lightness * color.saturation));
        float v1 = 2 * color.lightness - v2;
        r = unsigned(255.0f * hueToRGB(v1, v2, hue + (1.0f / 3)));
        g = unsigned(255.0f * hueToRGB(v1, v2, hue));
        b = unsigned(255.0f * hueToRGB(v1, v2, hue - (1.0f / 3)));
    }
    // NATIVE_SET_RGBW masks out bits not part of component, effectively doing range clamping
    return NATIVE_SET_RGBW(r, g, b, unsigned(color.white * 255.0f));
}

Color::Color(float h, float s, float l, float w)
    : hue(h)
    , saturation(s)
    , lightness(l)
    , white(w)
{

}
