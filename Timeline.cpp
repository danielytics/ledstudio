#include "Timeline.h"
#include "Compositor.h"
#include <cmath>

#define DECLARE_EASING(name, function) easingFunctions[name] = function;
#include "easing.h"

std::map<Easing, ease::Function> Timeline::easingFunctions;

Timeline::Timeline()
    : effectStarted(false)
{
    if (easingFunctions.size() == 0) {
        GENERATE_EASING_DECLARATIONS
    }
}
#undef DECLARE_EASING

void Timeline::clear()
{
    effects.clear();
}

void Timeline::addEffect(EffectInstance&& effect)
{
    effects.push_back(std::move(effect));
}

void Timeline::start()
{
    currentEffect = 0;
    effectStarted = false;
}

void Timeline::onTick (float currentTime, Compositor& compositor)
{
    while (currentEffect < effects.size()) {
        const EffectInstance& effect = effects[currentEffect];
        if (currentTime >= effect.startTime) {
            if (currentTime <= effect.endTime) {
                // Current effect is active
                if (!effectStarted) {
                    // Effect has just started, initialise its state
                    effectHandle = compositor.start(effect.id, effect.parameters);
                    // Mark effect as stated
                    effectStarted = true;
                }
                float duration = (effect.endTime - effect.startTime);
                float elapsed = currentTime - effect.startTime;
                float percentage = (elapsed / duration) *  effect.cycles;
                percentage = percentage - floorf(percentage); // wrap back to zero
                if (effect.invertedEasing) {
                    percentage = 1.0f - percentage;
                }
                compositor.setBlendMode(effect.blendMode);
                compositor.render(effectHandle, easingFunctions[effect.easing](percentage), effect.colorTable, effect.ledsTable);
                // Don't process any more this tick (timelines can only play one effect at once)
                return;
            } else {
                // Current effect has expired, cleanup
                compositor.stop(effectHandle);
                // Advance to next effect and mark it as not yet started
                ++currentEffect;
                effectStarted = false;
            }
        } else {
            // Effect not yet active
            return;
        }
    }
}
