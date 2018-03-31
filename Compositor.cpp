#include "Compositor.h"
#include "Timeline.h"
#include "LedStrip.h"
#include "json.hpp"

#include <cmath>

LedStrip::~LedStrip() {}

RenderEngine::~RenderEngine() {}
EffectRenderer::~EffectRenderer() {}

std::map<std::string, Easing> Compositor::easingFunctionsByName;
std::map<std::string, BlendMode> Compositor::blendModesByName;

#define TOSTR(x) #x
// Map string names to enums
#define DECLARE_EASING(name, function) easingFunctionsByName[TOSTR(name)] = name;
#include "easing.h"
void Compositor::populateEasingFunctionsNames() {
    if (easingFunctionsByName.size() == 0) {
        GENERATE_EASING_DECLARATIONS
    }
}
#undef DECLARE_EASING
#undef GENERATE_EASING_DECLARATIONS

Compositor::Compositor(LedStrip* strip)
    : strip(strip)
    , backbuffer(LedStrip::NUM_PIXELS)
{
    populateEasingFunctionsNames();
    if (blendModesByName.size() == 0) {
        blendModesByName["Normal"] = BLEND_NORMAL;
        blendModesByName["Screen"] = BLEND_SCREEN;
        blendModesByName["Addition"] = BLEND_ADDITION;
        blendModesByName["Subtract"] = BLEND_SUBTRACT;
        blendModesByName["Multiply"] = BLEND_MULTIPLY;
        blendModesByName["Divide"] = BLEND_DIVIDE;
        blendModesByName["Average"] = BLEND_AVERAGE;
        blendModesByName["Difference"] = BLEND_DIFFERENCE;
        blendModesByName["Hue"] = BLEND_HUE;
        blendModesByName["Saturation"] = BLEND_SATURATION;
        blendModesByName["Lightness"] = BLEND_LIGHTNESS;
        blendModesByName["White"] = BLEND_WHITE;
        blendModesByName["Color"] = BLEND_COLOR;
        blendModesByName["Dodge"] = BLEND_DODGE;
        blendModesByName["Burn"] = BLEND_BURN;
    }
    for (unsigned index=0; index<NUM_TIMELINES; index++) {
        timelines[index] = new Timeline;
    }
}

Compositor::~Compositor()
{
    for (auto effect : effects) {
        delete effect;
    }
    for (auto timeline : timelines) {
        delete timeline;
    }
}

EffectID Compositor::registerEffect(EffectRenderer* effect)
{
    EffectID id = effects.size();
    effects.push_back(effect);
    return id;
}

TableIndex Compositor::registerColorTable(ColorTable&& colorTable)
{
    TableIndex idx = colorTables.size();
    colorTables.push_back(std::move(colorTable));
    return idx;
}

TableIndex Compositor::registerLedsTable(LedsTable&& ledsTable)
{
    TableIndex idx = ledsTables.size();
    ledsTables.push_back(std::move(ledsTable));
    return idx;
}

EffectRenderer* Compositor::getEffect(EffectID id)
{
    return effects[id];
}

void Compositor::startCompositing()
{
    for (auto timeline : timelines) {
        timeline->start();
    }
    previousTime = 0;
}

void Compositor::setBlendMode(BlendMode mode)
{
    if (!disableBlendMode) {
        activeBlendMode = mode;
    }
}

EffectHandle Compositor::start(EffectID id, Params parameters)
{
    if (id < effects.size()) {
        EffectRenderer* renderer = effects[id];
        EffectHandle handle = renderer->instance(parameters);
        return (handle & 0xffff) | (id << 16);
    } else {
        return INVALID_HANDLE;
    }
}

void Compositor::render(EffectHandle handle, float percentage, TableIndex colorTable, TableIndex ledsTable)
{
    EffectID id = (handle >> 16) & 0xffff;
    if (id < effects.size()) {
        EffectRenderer* renderer = effects[id];
        renderer->render(handle & 0xffff, percentage, deltaTime, colorTables[colorTable], ledsTables[ledsTable], this);
    }
}

void Compositor::stop(EffectHandle handle)
{
    EffectID id = (handle >> 16) & 0xffff;
    if (id < effects.size()) {
        EffectRenderer* renderer = effects[id];
        renderer->dispose(handle & 0xffff);
    }
}

inline Color blend(const Color& bottom, const Color& top, BlendMode blendMode)
{
    switch (blendMode) {
    case BLEND_NORMAL:
        return top;
    case BLEND_ADDITION:
        return bottom + top;
    case BLEND_SUBTRACT:
        return bottom - top;
    case BLEND_MULTIPLY:
        return bottom * top;
    case BLEND_DIVIDE:
        return bottom / top;
    case BLEND_AVERAGE:
        return (bottom + top) * 0.5;
    case BLEND_DIFFERENCE:
        return Color(std::fabs(bottom.hue - top.hue), std::fabs(bottom.saturation - top.saturation), std::fabs(bottom.lightness - top.lightness), std::fabs(bottom.white - top.white));
    case BLEND_HUE:
        return Color(top.hue, bottom.saturation, bottom.lightness, bottom.white);
    case BLEND_SATURATION:
        return Color(bottom.hue, top.saturation, bottom.lightness, bottom.white);
    case BLEND_LIGHTNESS:
        return Color(bottom.hue, bottom.saturation, top.lightness, bottom.white);
    case BLEND_WHITE:
        return Color(bottom.hue, bottom.saturation, bottom.lightness, top.white);
    case BLEND_SCREEN:
        return (bottom.inverted() * top.inverted()).inverted();
    case BLEND_COLOR:
        return Color(top.hue, bottom.saturation, top.lightness, top.white);
    case BLEND_DODGE:
        return bottom * top.inverted();
    case BLEND_BURN:
        return bottom.inverted() * top;
    }
    return Color();
}

void Compositor::plot(Pixel pixel, const Color& color)
{
    backbuffer[pixel] = blend(backbuffer[pixel], color, activeBlendMode);
}

void Compositor::plot(const LedsTable& pixels, const Color& color)
{
    for (auto pixel : pixels) {
        backbuffer[pixel] = blend(backbuffer[pixel], color, activeBlendMode);
    }
}

void Compositor::plot(const LedsTable& pixels, const ColorTable& colors, bool wrap)
{
    for (std::size_t i=0; i < pixels.size() && (wrap || i < colors.size()); ++i) {
        Pixel pixel = pixels[i];
        backbuffer[pixel] = blend(backbuffer[pixel], colors[i % colors.size()], activeBlendMode);
    }
}

void Compositor::update(float time)
{
    deltaTime = time - previousTime;
    previousTime = time;
    // Clear backbuffer
    std::fill(backbuffer.begin(), backbuffer.end(), Color(0, 0, 0, 0));
    // Render each timeline

    activeBlendMode = BLEND_NORMAL; // First timeline cannot blend.
    disableBlendMode = true;
    for (auto timeline : timelines) {
        timeline->onTick(time, *this);
        disableBlendMode = false;
    }

    // Refresh
    std::vector<NativeColor> rawPixels;
    std::transform(backbuffer.begin(), backbuffer.end(), std::back_inserter(rawPixels), Color::toRGBW);
    strip->refresh(rawPixels);
}

#include <iostream>

using json = nlohmann::json;
void Compositor::load(const std::map<std::string, EffectID>& effectsByName, const std::string& source)
{
    std::map<std::string, TableIndex> colorsByName;
    std::map<std::string, TableIndex> ledsByName;

    std::cout << source << "\n\n";

    auto j = json::parse(source);

    // Register color tables
    colorTables.clear();
    colorsByName["white"] = registerColorTable({Color(0, 1, 1)});
    colorsByName["off"] = registerColorTable({Color(0, 0, 0)});
    colorsByName["superwhite"] = registerColorTable({Color(0, 1, 1,1)});
    colorsByName["red"] = registerColorTable({Color(0, 1, 0.5)});
    colorsByName["green"] = registerColorTable({Color(120, 1, 0.5)});
    colorsByName["blue"] = registerColorTable({Color(240, 1, 0.5)});
    for (auto entry : j["color_tables"].items()) {
        auto name = entry.key();
        std::vector<Color> table;
        for (auto hslw : entry.value()) {
            table.push_back({hslw["h"].get<float>(), hslw["s"].get<float>(), hslw["l"].get<float>(), hslw["w"].get<float>()});
        }
        colorsByName[name] = registerColorTable(std::move(table));
    }

    // Register LED tables
    ledsTables.clear();
    {
        Pixel idx = 0;
        std::vector<Pixel> all;
        std::generate_n(std::back_inserter(all), LedStrip::NUM_PIXELS, [&idx](){return idx++;});
        ledsByName["all"] = registerLedsTable(std::move(all));

        idx = 0;
        std::vector<Pixel> j;
        std::generate_n(std::back_inserter(j), 52, [&idx](){return idx++;});
        ledsByName["J"] = registerLedsTable(std::move(j));

        idx = 52;
        std::vector<Pixel> g;
        std::generate_n(std::back_inserter(g), LedStrip::NUM_PIXELS - idx, [&idx](){return idx++;});
        ledsByName["G"] = registerLedsTable(std::move(g));
    }
    for (auto entry : j["led_tables"].items()) {
        auto name = entry.key();
        std::vector<Pixel> table;
        for (auto pixel : entry.value()) {
            table.push_back(pixel.get<Pixel>());
        }
        ledsByName[name] = registerLedsTable(std::move(table));
    }

    // Load timeline data
    for (Timeline* timeline : timelines) {
        timeline->clear();
    }
    for (auto entry : j["tracks"].items()) {
        unsigned track = unsigned(std::stoi(entry.key()));
        auto effects = entry.value();
        for (auto effect : effects) {
            float start = effect["start_time"];
            float end = effect["end_time"];
            EffectID effectId = effectsByName.at(effect["name"]);
            float cycles = effect["cycles"];
            Easing easing = easingFunctionsByName.at(effect["easing"]);
            TableIndex colorTable = colorsByName.at(effect["colors"]);
            TableIndex ledsTable = ledsByName.at(effect["leds"]);
            BlendMode blendMode = blendModesByName.at(effect["blend_mode"]);
            bool invertedEasing = effect["inverted_easing"];
            Params params;
            for (auto param : effect["props"].items()) {
                params[param.key()] = param.value();
            }
            timelines[track]->addEffect({start, end, effectId, cycles, easing, invertedEasing, colorTable, ledsTable, blendMode, params});
        }
    }
}
