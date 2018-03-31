

#include "LedStrip.h"
#include "Compositor.h"
#include "SparkleEffectRenderer.h"
#include "FadeEffectRenderer.h"
#include "LightupEffectRenderer.h"

#include <map>
#include <string>
#include <fstream>

#include <stdint.h>

extern "C" {
	int ledstrip_init();
	void ledstrip_term();
	int ledstrip_refresh(uint32_t* data, uint32_t n);
	int ledstrip_clear();
	void ledstrip_yield();
	int ledstrip_should_term();
};

class PhysicalLedStrip : public LedStrip {
public:
	PhysicalLedStrip();
    ~PhysicalLedStrip();

    void init();
    void term();
    void reset();
    void refresh(const std::array<NativeColor, NUM_PIXELS>&& pixels);
};

PhysicalLedStrip::PhysicalLedStrip()
{
}

PhysicalLedStrip::~PhysicalLedStrip()
{
}

void PhysicalLedStrip::init()
{
	ledstrip_init();
}

void PhysicalLedStrip::term()
{
	ledstrip_clear();
	ledstrip_term();
}

void PhysicalLedStrip::reset()
{
	ledstrip_clear();
}

void PhysicalLedStrip::refresh(const std::array<NativeColor, NUM_PIXELS>&& pixels)
{
	ledstrip_refresh((uint32_t*)(pixels.data()), uint32_t(pixels.size()));
}

enum RunState {
	RUNSTATE_WAITING,
	RUNSTATE_RUNNING,
	RUNSTATE_RESTART,
	RUNSTATE_TERMINATE
};

int main(int argc, char *argv[])
{
    Compositor compositor(new PhysicalLedStrip());

    std::map<std::string, EffectID> effectsMap {
		{"sparkle", compositor.registerEffect(new SparkleEffectRenderer)}, 
		{"fade", compositor.registerEffect(new FadeEffectRenderer)},
		{"lightup", compositor.registerEffect(new LightupEffectRenderer)}
	};
	
	{
		std::ifstream in(argc > 1 ? argv[1] : "default.json");
		std::string contents((std::istreambuf_iterator<char>(in)), 
		std::istreambuf_iterator<char>());
		compositor.load(effectsMap, contents);
	}

	RunState prevState = RUNSTATE_WAITING;
	RunState currentState = RUNSTATE_WAITING;
	
	do {
		if (currentState == RUNSTATE_RUNNING) {
			if (prevState != RUNSTATE_RUNNING) {
				compositor.startCompositing();
			}
			compositor.update(1.0f);
		}
		prevState = currentState;
		if (ledstrip_should_term()) {
			currentState = RUNSTATE_TERMINATE;
		}
		ledstrip_yield();
	} while (currentState != RUNSTATE_TERMINATE);
}
