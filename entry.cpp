
#include <signal.h>
#include <cstdint>
#include <time.h>
#include <unistd.h>

#include <string>
#include <fstream>

#include "Compositor.h"
#include "PhysicalLedStrip.h"
#include "SparkleEffectRenderer.h"
#include "FadeEffectRenderer.h"
#include "LightupEffectRenderer.h"

enum RunState {
	RUNSTATE_WAITING,
	RUNSTATE_RUNNING,
	RUNSTATE_RESTART,
	RUNSTATE_RELOAD,
	RUNSTATE_TERMINATE
};
volatile RunState nextState;

static void ctrl_c_handler(int signum)
{
    if (signum == 2 || signum == 15) {
	printf("Interrupted, stopping.\n");
	nextState = RUNSTATE_TERMINATE;
    } else if (signum == 10) {
	printf("Reload requested.\n");
    	nextState = RUNSTATE_RELOAD;
    }
}

static void setup_handlers(void)
{
    struct sigaction sa; 
    sa.sa_handler = ctrl_c_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
}



int main(int argc, char *argv[])
{
	int i;
	struct timespec start_ts, current_ts;
	float currentTime = 0.0f;
	
    setup_handlers();
    PhysicalLedStrip strip;
    strip.init();
	
    Compositor compositor(&strip);

    std::map<std::string, EffectID> effectsMap {
		{"sparkle", compositor.registerEffect(new SparkleEffectRenderer)}, 
		{"fade", compositor.registerEffect(new FadeEffectRenderer)},
		{"lightup", compositor.registerEffect(new LightupEffectRenderer)}
	};
	

	RunState prevState = RUNSTATE_WAITING;
	RunState currentState = RUNSTATE_RELOAD;
	nextState = RUNSTATE_RUNNING;

	printf("Ready...\n");
	usleep(250000); // 1/4 second pause
	do {
		if (currentState == RUNSTATE_RUNNING) {
			if (prevState != RUNSTATE_RUNNING) {
				printf("Starting.\n");
				compositor.startCompositing();
				clock_gettime(CLOCK_MONOTONIC, &start_ts);
			}
			clock_gettime(CLOCK_MONOTONIC, &current_ts);
			currentTime = ( current_ts.tv_sec - start_ts.tv_sec ) + ( current_ts.tv_nsec - start_ts.tv_nsec ) * 1E-9;
			compositor.update(currentTime);
		} else if (currentState == RUNSTATE_RELOAD) {
			printf("Loading configuration.\n");
			nextState = RUNSTATE_RUNNING;
			std::ifstream in(argc > 1 ? argv[1] : "default.json");
			std::string contents((std::istreambuf_iterator<char>(in)), 
			std::istreambuf_iterator<char>());
			compositor.load(effectsMap, contents);
			if(prevState == RUNSTATE_WAITING) currentState = prevState;
		}
		//printf("prev: %d, next: %d, current: %d\n", prevState, nextState, currentState);
		prevState = currentState;
		currentState = nextState;
		usleep(1000000 / 90); // Maximum of 90 refresh's per second
	} while (currentState != RUNSTATE_TERMINATE);

    strip.term();
    return 0;
}
