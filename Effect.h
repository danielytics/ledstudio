#ifndef EFFECT_H
#define EFFECT_H


#include <vector>
#include <map>
#include <string>
#include "Color.h"

typedef uint8_t Pixel;
typedef std::size_t EffectID;
typedef std::size_t EffectHandle;
typedef std::size_t TableIndex;

enum {INVALID_HANDLE=EffectHandle(-1)};

typedef std::vector<Color> ColorTable;
typedef std::vector<Pixel> LedsTable;
typedef std::map<std::string, float> Params;



#define DECLARE_EASING(name, func) name,
#include "easing.h"

enum Easing {
    GENERATE_EASING_DECLARATIONS
    NumEasingFuncs
};

#undef DECLARE_EASING

enum BlendMode {
    BLEND_NORMAL,
    BLEND_ADDITION,
    BLEND_SUBTRACT,
    BLEND_MULTIPLY,
    BLEND_DIVIDE,
    BLEND_AVERAGE,
    BLEND_DIFFERENCE,
    BLEND_HUE,
    BLEND_SATURATION,
    BLEND_LIGHTNESS,
    BLEND_WHITE,
    BLEND_SCREEN,
    BLEND_COLOR,
    BLEND_DODGE,
    BLEND_BURN
};

struct EffectInstance {
    float startTime;
    float endTime;
    EffectID id;
    float cycles;
    Easing easing;
    bool invertedEasing;
    TableIndex colorTable;
    TableIndex ledsTable;
    BlendMode blendMode;
    Params parameters;
};

#define F_LIMIT 0.001f
#define F_ONE (1.0f - F_LIMIT)
#define F_ZERO F_LIMIT

#endif // EFFECT_H
