#include "SparkleEffectRenderer.h"
#include "Random.h"
#include <utility>

SparkleEffectRenderer::SparkleEffectRenderer()
    : nextFreeHandle(0)
{

}

Params SparkleEffectRenderer::parameters()
{
    return {
        {"density", 0.1f},
        {"frequency", 10.0f}
    };
}

EffectHandle SparkleEffectRenderer::instance(const Params& params)
{
    EffectHandle handle = nextFreeHandle++;
    float density = get(params, "density");
    float frequency = get(params, "frequency");
    state.emplace(std::make_pair(handle, SparkleState{{}, density, frequency, 0.0f}));
    return handle;
}

void SparkleEffectRenderer::dispose(EffectHandle handle)
{
    state.erase(handle);
}

void SparkleEffectRenderer::render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine)
{
    SparkleState& s = state[handle];
    s.timer += delta * s.frequency * percentage;
    if (s.timer >= 1.0f) {
        s.timer = 0;
        // Toggle
        s.litPixels.clear();
        unsigned count = unsigned(pixels.size() * s.density);
        unsigned attempts = 50;
        while (s.litPixels.size() < count) {
            s.litPixels.insert(std::make_pair(Pixel(Random::value(unsigned(pixels.size()))), std::size_t(Random::value(unsigned(colorTable.size())))));
            if (--attempts == 0) break; // Give up after a while
        }
    }
    for (auto pair : s.litPixels) {
        engine->plot(pixels[pair.first], colorTable[pair.second]);
    }
}
