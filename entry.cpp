
#include <signal.h>
#include <cstdint>
#include <time.h>
#include <unistd.h>

#include <wiringPi.h>

#include <string>
#include <fstream>

#include "Compositor.h"
#include "PhysicalLedStrip.h"
#include "SparkleEffectRenderer.h"
#include "FadeEffectRenderer.h"
#include "LightupEffectRenderer.h"
#include "SwipeEffectRenderer.h"

enum RunState {
	RUNSTATE_WAITING,
	RUNSTATE_RUNNING,
	RUNSTATE_RESTART,
	RUNSTATE_RELOAD,
	RUNSTATE_TERMINATE
};
static volatile RunState nextState;

static void ctrl_c_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
		printf("Interrupted, stopping.\n");
		nextState = RUNSTATE_TERMINATE;
    } else if (signum == SIGUSR1) {
		printf("Reload requested.\n");
		nextState = RUNSTATE_RELOAD;
    } else if (signum == SIGUSR2) {
		printf("Restart requested.\n");
		nextState = RUNSTATE_RESTART;
    }
}

static void setup_handlers(void)
{
	struct sigaction sa;
	sa.sa_handler = ctrl_c_handler;
	sigaction(SIGINT, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGUSR1, &sa, 0);
	sigaction(SIGUSR2, &sa, 0);
}

#define BUTTON_PIN 14


int main(int argc, char *argv[])
{
	struct timespec start_ts, current_ts;
	float currentTime = 0.0f;

	wiringPiSetupGpio();
	pinMode (BUTTON_PIN, INPUT) ;
	pullUpDnControl(BUTTON_PIN, PUD_UP);
	
    setup_handlers();
    PhysicalLedStrip strip;
    strip.init();
	
    Compositor compositor(&strip);

    std::map<std::string, EffectID> effectsMap {
		{"sparkle", compositor.registerEffect(new SparkleEffectRenderer)}, 
		{"fade", compositor.registerEffect(new FadeEffectRenderer)},
		{"lightup", compositor.registerEffect(new LightupEffectRenderer)},
		{"swipe", compositor.registerEffect(new SwipeEffectRenderer)}
	};
	

	RunState prevState = RUNSTATE_WAITING;
	RunState currentState = RUNSTATE_RELOAD;
	nextState = RUNSTATE_WAITING;

	usleep(250000); // 1/4 second pause
	do {
		if (digitalRead(BUTTON_PIN) == LOW) {
			// If button is pressed and we are waiting, then we are no longer waiting
			if (currentState == RUNSTATE_WAITING) {
				nextState = RUNSTATE_RUNNING;
			} else {
				// Otherwise, we want to resart.
				nextState = RUNSTATE_RESTART;
			}
		}
		if (currentState == RUNSTATE_RUNNING) {
			if (prevState != RUNSTATE_RUNNING) {
				printf("Starting.\n");
				compositor.startCompositing();
				clock_gettime(CLOCK_MONOTONIC, &start_ts);
			}
			clock_gettime(CLOCK_MONOTONIC, &current_ts);
			currentTime = float(( current_ts.tv_sec - start_ts.tv_sec ) + ( current_ts.tv_nsec - start_ts.tv_nsec ) * 1E-9);
			compositor.update(currentTime);
		} else if (currentState == RUNSTATE_RELOAD) {
			printf("Loading configuration.\n");
			std::ifstream in(argc > 1 ? argv[1] : "default.json");
			std::string contents((std::istreambuf_iterator<char>(in)), 
			std::istreambuf_iterator<char>());
			compositor.load(effectsMap, contents);
			nextState = prevState;
		} else if (currentState == RUNSTATE_RESTART) {
			printf("Restarting.\n");
			nextState = RUNSTATE_RUNNING;
		}
		prevState = currentState;
		currentState = nextState;
		usleep(1000000 / 120); // Maximum of 120 refresh's per second
	} while (currentState != RUNSTATE_TERMINATE);

    printf("Stopping.\n");
    strip.term();
    return 0;
}
