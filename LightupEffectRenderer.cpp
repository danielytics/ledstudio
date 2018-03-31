#include "LightupEffectRenderer.h"

LightupEffectRenderer::LightupEffectRenderer()
    : nextFreeHandle(0)
{

}

Params LightupEffectRenderer::parameters()
{
    return {
        {"threshold", 1.0f},
        {"repeat colours", 0}
    };
}

EffectHandle LightupEffectRenderer::instance(const Params& params)
{
    EffectHandle handle = nextFreeHandle++;
    float threshold = get(params, "threshold");
    bool wrap = get(params, "repeat colours") > 0.5f;
    state.emplace(std::make_pair(handle, LightupState{threshold, wrap}));
    return handle;
}

void LightupEffectRenderer::dispose(EffectHandle handle)
{
    state.erase(handle);
}

void LightupEffectRenderer::render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine)
{
    LightupState& s = state[handle];
    if (percentage >= s.threshold - F_LIMIT) {
        engine->plot(pixels, colorTable, s.wrap);
    }
}
