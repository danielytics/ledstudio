#ifndef SWIPEEFFECT_H
#define SWIPEEFFECT_H

#include "EffectRenderer.h"
#include <map>

struct SwipeState {
    float moveX;
    float moveY;
    int trail;
    TableIndex current; // current color index
};

class SwipeEffectRenderer : public EffectRenderer
{
public:
    SwipeEffectRenderer();

    Params parameters();
    EffectHandle instance(const Params& parameters);
    void dispose(EffectHandle handle);
    void render(EffectHandle handle, float percentage, float delta, const ColorTable& colorTable, const LedsTable& pixels, RenderEngine* engine);

private:
    std::map<EffectHandle, SwipeState> state;
    EffectHandle nextFreeHandle;
    static std::vector<LedsTable> pixelMapColumns;
    static std::vector<LedsTable> pixelMapRows;
};

#endif // SWIPEEFFECT_H
