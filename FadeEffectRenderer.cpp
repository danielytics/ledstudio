#include "FadeEffectRenderer.h"
#include <utility>

FadeEffectRenderer::FadeEffectRenderer()
    : nextFreeHandle(0)
{

}

Params FadeEffectRenderer::parameters()
{
    return {};
}

EffectHandle FadeEffectRenderer::instance(const Params&)
{
    EffectHandle handle = nextFreeHandle++;
    state.emplace(std::make_pair(handle, FadeState{0}));
    return handle;
}

void FadeEffectRenderer::dispose(EffectHandle handle)
{
    state.erase(handle);
}

void FadeEffectRenderer::render(EffectHandle handle, float percentage, float, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine)
{
    FadeState& s = state[handle];
    const Color& current = colorTable[s.current];
    if (colorTable.size() == 1) {
        engine->plot(pixels, Color().lerp(current, percentage));
    } else if (s.current + 1 < colorTable.size()) {
        const Color& next = colorTable[s.current + 1];
        engine->plot(pixels, current.lerp(next, percentage));
    }
    if (percentage >= 0.95f) {
        s.current = (s.current + 1) % colorTable.size();
    }
}
