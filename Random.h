#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
public:
    Random();

    static float value();
    static float value(float max);
    static float value(float min, float max);
    static unsigned value(unsigned max);
    static unsigned value(unsigned min, unsigned max);
    static int value(int max);
    static int value(int min, int max);

private:
    static Random random;
    std::random_device r;
    std::minstd_rand engine;
    std::uniform_real_distribution<> real_distribution;
    std::uniform_int_distribution<> int_distribution;
};

#endif // RANDOM_H
