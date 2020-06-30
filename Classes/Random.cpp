#include "Random.h"

Random* ISingle<Random>::instance = new Random();
Random::Random()
{

}

Random::~Random()
{
}

void Random::init(int seed)
{
	e.seed(seed);
	
}
