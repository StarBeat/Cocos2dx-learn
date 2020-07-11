#pragma once
#include "ISingle.h"
#include "cocos2d.h"
#include <unordered_map>
#include <stack>
#include "Asteroid.h"

using cocos2d::Rect;

class AsteroidManager : public ISingle<AsteroidManager>
{
public:
	AsteroidManager();
	~AsteroidManager();
	void spwanAsteriod();
	void init();
	void update(float dt);
public:
	static const int AREA_COUNT = 4;
	int _asteroidCount;
	Rect _areas[AREA_COUNT];
	std::stack<Asteroid> _pool;
	struct _Pair
	{
		int playercount;
		int asteroidcount;
	};
	std::unordered_map<int, _Pair> _aid2count;
};