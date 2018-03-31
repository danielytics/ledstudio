#ifndef TIMELINE_H
#define TIMELINE_H

#include <vector>
#include <map>

#include "Effect.h"

class Compositor;

class Timeline
{
public:
    Timeline();

    void clear();
    void addEffect(EffectInstance&& effect);

    void start();
    void onTick (float time, Compositor& compositor);

private:
    std::vector<EffectInstance> effects;
    unsigned currentEffect;
    bool effectStarted;
    EffectHandle effectHandle;
    static std::map<Easing, ease::Function> easingFunctions;
};

#endif // TIMELINE_H
