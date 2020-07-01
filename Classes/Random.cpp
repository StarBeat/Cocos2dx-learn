#include "Random.h"
#include "cocos2d.h"
Random* ISingle<Random>::instance = new Random();
Random::Random()
{

}

Random::~Random()
{
}

void Random::init(int seed)
{
	CCLOG("Random Seed %d\n", seed);
	e.seed(seed);
}
