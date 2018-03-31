#ifndef SPARKLEEFFECTRENDERER_H
#define SPARKLEEFFECTRENDERER_H

#include "EffectRenderer.h"
#include <map>
#include <set>

struct SparkleState {
    std::set<std::pair<Pixel, std::size_t>> litPixels;
    float density;
    float frequency;
    float timer;
};

class SparkleEffectRenderer : public EffectRenderer
{
public:
    SparkleEffectRenderer();

    Params parameters();
    EffectHandle instance(const Params& parameters);
    void dispose(EffectHandle handle);
    void render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine);

private:
    std::map<EffectHandle, SparkleState> state;
    EffectHandle nextFreeHandle;
};

#endif // SPARKLEEFFECTRENDERER_H
