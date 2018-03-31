#ifndef LIGHTUPEFFECTRENDERER_H
#define LIGHTUPEFFECTRENDERER_H

#include "EffectRenderer.h"

struct LightupState {
    float threshold;
    bool wrap;
};

class LightupEffectRenderer : public EffectRenderer
{
public:
    LightupEffectRenderer();

    Params parameters();
    EffectHandle instance(const Params& parameters);
    void dispose(EffectHandle handle);
    void render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine);

private:
    std::map<EffectHandle, LightupState> state;
    EffectHandle nextFreeHandle;
};

#endif // LIGHTUPEFFECTRENDERER_H
