#pragma once
#include "ISingle.h"
#include <random>

struct Random : public ISingle<Random>
{
	std::default_random_engine e;
	int seed;
	Random();
	~Random();
	template<typename T>
	T get(T min, T max) 
	{
		std::uniform_real_distribution<float> u(min, max);
		return u(e);
	}
	void init(int seed);
};