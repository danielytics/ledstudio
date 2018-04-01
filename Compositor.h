#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <array>
#include <vector>

#include "Color.h"
#include "Effect.h"
#include "EffectRenderer.h"
#include "Timeline.h"

class LedStrip;

class Compositor : public RenderEngine
{
public:
    enum {NUM_TIMELINES=6};
    Compositor(LedStrip* strip);
    ~Compositor();

    EffectID registerEffect(EffectRenderer* effect);
    TableIndex registerColorTable(ColorTable&& colorTable);
    TableIndex registerLedsTable(LedsTable&& ledsTable);
    EffectRenderer* getEffect(EffectID id);

    void setBlendMode(BlendMode mode);

    EffectHandle start(EffectID id, Params parameters);
    void render(EffectHandle handle, float percentage, TableIndex colorTable, TableIndex ledsTable);
    void stop(EffectHandle handle);

    void plot(Pixel pixel, const Color& color);
    void plot(const LedsTable& pixels, const Color& color);
    void plot(const LedsTable& pixels, const ColorTable& colors, bool wrap);

    void startCompositing();
    void update(float time);

    void load(const std::map<std::string, EffectID>& effectsByName, const std::string& source);

private:
    std::array<Timeline*, NUM_TIMELINES> timelines;
    LedStrip* strip;
    std::vector<Color> backbuffer;
    std::vector<EffectRenderer*> effects;
    std::vector<ColorTable> colorTables;
    std::vector<LedsTable> ledsTables;
    BlendMode activeBlendMode;
    bool disableBlendMode;
    static std::map<std::string, Easing> easingFunctionsByName;
    static std::map<std::string, BlendMode> blendModesByName;
    static void populateEasingFunctionsNames();
    float deltaTime;
    float previousTime;
};

#endif // COMPOSITOR_H
