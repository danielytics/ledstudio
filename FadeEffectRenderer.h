#ifndef FADEEFFECTRENDERER_H
#define FADEEFFECTRENDERER_H

#include "EffectRenderer.h"
#include <map>

struct FadeState {
    std::size_t current;
};

class FadeEffectRenderer : public EffectRenderer
{
public:
    FadeEffectRenderer();

    Params parameters();
    EffectHandle instance(const Params& parameters);
    void dispose(EffectHandle handle);
    void render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine);

private:
    std::map<EffectHandle, FadeState> state;
    EffectHandle nextFreeHandle;
};

#endif // FADEEFFECTRENDERER_H
