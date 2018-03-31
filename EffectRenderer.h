#ifndef EFFECTRENDERER_H
#define EFFECTRENDERER_H

#include "Effect.h"

class RenderEngine {
public:
    virtual ~RenderEngine()=0;
    virtual void plot(Pixel pixel, const Color& color)=0;
    virtual void plot(const LedsTable& pixels, const Color& color)=0;
    virtual void plot(const LedsTable& pixels, const ColorTable& colors, bool wrap)=0;
};

class EffectRenderer {
public:
    virtual ~EffectRenderer()=0;

    virtual Params parameters()=0;

    virtual EffectHandle instance(const Params& parameters)=0;
    virtual void dispose(EffectHandle handle)=0;
    virtual void render(EffectHandle handle, float percentage, float deltaTime, const ColorTable& colorTable, const LedsTable& ledsTable, RenderEngine* engine)=0;

protected:
    inline float get(const Params& m, const std::string& key)
    {
        typename Params::const_iterator it = m.find( key );
        if (it == m.end())
            return parameters()[key];
        return it->second;
    }
};

#endif // EFFECTRENDERER_H
