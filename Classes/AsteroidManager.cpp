#include "AsteroidManager.h"
#include "GameScene.h"
#include "Random.h"
#include "PlayerManager.h"
#include "IPlayer.h"

using namespace cocos2d;

AsteroidManager* ISingle< AsteroidManager>::instance = new AsteroidManager();

AsteroidManager::AsteroidManager():_asteroidCount(0)
{

}

AsteroidManager::~AsteroidManager()
{
}

void AsteroidManager::spwanAsteriod()
{
	auto gs = GameScene::gameScene;
	auto r = Random::Instane();
	_asteroidCount += 100;
	float x, y;
	int t;
	for (auto i : _aid2playercount)
	{
		if (i.second >= 2)
		{
			for (size_t j = 0; j < 100; j++)
			{
				x = r->get<float>(_areas[i.first].getMinX(), _areas[i.first].getMaxX());
				y = r->get<float>(_areas[i.first].getMinY(), _areas[i.first].getMaxY());
				t = r->get<int>(0, 3);
				auto asteroid = new Asteroid(x, y, r->get<int>(10, 50), t);
				gs->addChild(asteroid->getNode());
			}
		}
		else if (i.second == 1)
		{
			for (size_t j = 0; j < 100; j++)
			{
				x = r->get<float>(_areas[i.first].getMinX(), _areas[i.first].getMaxX());
				y = r->get<float>(_areas[i.first].getMinY(), _areas[i.first].getMaxY());
				t = r->get<int>(0, 2);
				auto asteroid = new Asteroid(x, y, r->get<int>(10, 50), t);
				gs->addChild(asteroid->getNode());
			}
		}
	}

}

void AsteroidManager::init()
{
	auto rectsize = GameScene::gameScene->gameSize / 4;
	GameScene::gameScene->getScheduler()->scheduleUpdate(this, 0, false);
	_areas[0] = { Vec2{-rectsize.x, 0},Size(rectsize) };
	_areas[1] = { Vec2{0, 0},Size(rectsize) };
	_areas[2] = { Vec2{-rectsize.x, -rectsize.y },Size(rectsize) };
	_areas[3] = { Vec2{0, -rectsize.y},Size(rectsize) };
}

void AsteroidManager::update(float dt)
{
	auto players = PlayerManager::Instane()->_players;
	if (players.size() <= 1)
	{
		return;
	}
	for (auto i : _aid2playercount)
	{
		i.second = 0;
	}
	for (auto i : players)
	{
		auto pos = i->operator IPrimitive* ()->getPosition();
		for (size_t j = 0; j < AREA_COUNT; j++)
		{
			if (_areas[j].containsPoint(pos))
			{
				++_aid2playercount[j];
			}
		}
	}
	if (_asteroidCount < 100)
	{
		spwanAsteriod();
	}
}
