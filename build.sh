#/bin/sh
g++ -O3 -I../rpi_ws281x PhysicalLedStrip.cpp Color.cpp Compositor.cpp FadeEffectRenderer.cpp LightupEffectRenderer.cpp Random.cpp SparkleEffectRenderer.cpp Timeline.cpp easing.cpp entry.cpp -o jg ../rpi_ws281x/libws2811.a
