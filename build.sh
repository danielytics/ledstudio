#/bin/sh
for source in Color.cpp Compositor.cpp FadeEffectRenderer.cpp LightupEffectRenderer.cpp Random.cpp SparkleEffectRenderer.cpp Timeline.cpp easing.cpp jgmain.cpp ;
do
	echo "Building $source"
    g++ -O3 -fno-rtti -std=c++14 -static-libstdc++ -c $source
done
echo "Building ledstrip.c"
gcc -O3 -I../rpi_ws281x -c ledstrip.c
echo "Linking"
g++  -fno-rtti -static-libstdc++ -o jg *.o ../rpi_ws281x/libws2811.a
echo "Done"
