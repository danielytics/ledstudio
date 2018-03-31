#include "PhysicalLedStrip.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ws2811.h"


#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE              SK6812_STRIP_GRBW

#define WIDTH                   8
#define HEIGHT                  8
#define LED_COUNT             131

int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;

ws2811_t ledstring;

PhysicalLedStrip::PhysicalLedStrip()
{
}

PhysicalLedStrip::~PhysicalLedStrip()
{
}

void PhysicalLedStrip::init()
{
    ledstring.freq                  = TARGET_FREQ;
    ledstring.dmanum                = DMA;
    ledstring.channel[0].gpionum    = GPIO_PIN;
    ledstring.channel[0].invert     = 0;
    ledstring.channel[0].count      = LED_COUNT;
    ledstring.channel[0].strip_type = STRIP_TYPE;
    ledstring.channel[0].brightness = 10;
    ledstring.channel[0].wshift     = 0;
    ledstring.channel[0].rshift     = 0;
    ledstring.channel[0].gshift     = 0;
    ledstring.channel[0].bshift     = 0;
    ledstring.channel[0].gamma      = 0;
    ledstring.channel[1].gpionum    = 0;
    ledstring.channel[1].invert     = 0;
    ledstring.channel[1].count      = 0;
    ledstring.channel[1].strip_type = 0;
    ledstring.channel[1].brightness = 0;
    ledstring.channel[1].wshift     = 0;
    ledstring.channel[1].rshift     = 0;
    ledstring.channel[1].gshift     = 0;
    ledstring.channel[1].bshift     = 0;
    ledstring.channel[1].gamma      = 0;

	ws2811_return_t ret;
    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        return;
    }
}

void PhysicalLedStrip::term()
{
	reset();
    ws2811_fini(&ledstring);
}

void PhysicalLedStrip::reset()
{
	for (std::size_t i=0; i<LED_COUNT; ++i) {
		ledstring.channel[0].leds[i] = 0;
	}
	ws2811_render(&ledstring);
}

void PhysicalLedStrip::refresh(const std::vector<NativeColor>& pixels)
{
	for (unsigned i=0; i<pixels.size(); ++i) {
		ledstring.channel[0].leds[i] = pixels[i];
	}
	ws2811_return_t ret;
	if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS) {
		fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
	}
}
