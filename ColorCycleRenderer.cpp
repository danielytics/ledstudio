#include "ColorCycleRenderer.h"
#include "SwipeEffectRenderer.h"

#include <set>

ColorCycleRenderer::ColorCycleRenderer()
{

}

Params ColorCycleRenderer::parameters()
{
    return {};
}

EffectHandle ColorCycleRenderer::instance(const Params& params)
{
    EffectHandle handle = nextFreeHandle++;
    float moveX = get(params, "x");
    float moveY = get(params, "y");
    state.emplace(std::make_pair(handle, ColorCycleState{0}));
    return handle;
}

void ColorCycleRenderer::dispose(EffectHandle handle)
{
    state.erase(handle);
}

void ColorCycleRenderer::render(EffectHandle handle, float percentage, float, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine)
{
    ColorCycleState& s = state[handle];

    // Get the current and next colors to fade between
    Color current = colorTable[s.current];
    Color next;
    if (colorTable.size() == 1) {
        next = current;
        current = Color();
    } if (s.current + 1 < colorTable.size()) {
        next = colorTable[s.current + 1];
    }
    // Advance colors if reaching the end of the percentage
    if (percentage >= 0.95f) {
        s.current = (s.current + 1) % colorTable.size();
    }

    // Draw, fading between colors
    std::size_t currentIndex = std::size_t(percentage * float(SwipeEffectRenderer::pixelMapRows.size()));
    for (std::size_t row=currentIndex; row < SwipeEffectRenderer::pixelMapRows.size(); ++row) {
         engine->plot(SwipeEffectRenderer::pixelMapRows[row], current.lerp(next, percentage));
    }
}
