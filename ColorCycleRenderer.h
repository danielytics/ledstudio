#ifndef RAINBOWCYCLERENDERER_H
#define RAINBOWCYCLERENDERER_H

#include "EffectRenderer.h"

struct ColorCycleState {
    TableIndex current; // current position
};

class ColorCycleRenderer : public EffectRenderer
{
public:
    ColorCycleRenderer();

    Params parameters();
    EffectHandle instance(const Params& parameters);
    void dispose(EffectHandle handle);
    void render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine);
private:
    std::map<EffectHandle, ColorCycleState> state;
    EffectHandle nextFreeHandle;
};

#endif // RAINBOWCYCLERENDERER_H
