#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
    Color() : Color(0, 0, 0, 0) {}
    Color(float h, float s, float l, float w=0.0f);

    float hue;
    float saturation;
    float lightness;
    float white;

    inline Color operator+ (const Color& other) const {
        return Color(hue + other.hue, saturation + other.saturation, lightness + other.lightness, white + other.white);
    }
    inline Color operator- (const Color& other) const {
        return Color(hue - other.hue, saturation - other.saturation, lightness - other.lightness, white - other.white);
    }
    inline Color operator* (const Color& other) const {
        return Color(hue * other.hue, saturation * other.saturation, lightness * other.lightness, white * other.white);
    }
    inline Color operator/ (const Color& other) const {
        float h = other.saturation > 0.0f ? other.saturation : 0.0001f;
        float s = other.saturation > 0.0f ? other.saturation : 0.0001f;
        float l = other.saturation > 0.0f ? other.saturation : 0.0001f;
        float w = other.saturation > 0.0f ? other.saturation : 0.0001f;
        return Color(hue / h, saturation / s, lightness / l, white / w);
    }
    inline Color operator* (float constant) const {
        return Color(hue + constant, saturation + constant, lightness + constant, white + constant);
    }
    inline Color inverted() const {
        return Color(360.0f-hue, 1.0f-saturation, 1.0f-lightness, 1.0f-white);
    }

    inline Color lerp(const Color& other, float value) const {
        return Color(lerp(hue, other.hue, value), lerp(saturation, other.saturation, value), lerp(lightness, other.lightness, value), lerp(white, other.white, value));
    }

    static unsigned toRGBW(const Color& color);
private:
    inline float lerp(float a, float b, float f) const {
        return a + f * (b - a);
    }
};

#endif // COLOR_H
