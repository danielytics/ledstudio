#include "Random.h"

Random Random::random;

Random::Random()
    : engine(r())
{

}

float Random::value()
{
    return float(random.real_distribution(random.engine));
}

float Random::value(float max)
{
    return float(random.real_distribution(random.engine)) * max;
}

float Random::value(float min, float max)
{
    return (float(random.real_distribution(random.engine)) * (max - min)) + min;
}

unsigned Random::value(unsigned max)
{
    return unsigned(random.int_distribution(random.engine) % max);
}

unsigned Random::value(unsigned min, unsigned max)
{
    return unsigned((value() * (max - min)) + min);
}

int Random::value(int max)
{
    return int(random.int_distribution(random.engine) % max);
}

int Random::value(int min, int max)
{
    return int((value() * (max - min)) + min);
}
