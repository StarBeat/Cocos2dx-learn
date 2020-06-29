#pragma once
#include "ISingle.h"
#include <random>

struct Random : public ISingle<Random>
{
	std::default_random_engine e;
	int seed;
	Random();
	~Random();
	void init(int seed);
};