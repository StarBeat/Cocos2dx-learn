#include "LocalPlayer.h"
using namespace cocos2d;

LocalPlayer* LocalPlayer::create(Vec2 pos, Vec2 rot)
{
	_position = pos;
	_rotation = rot;
	auto ins = new LocalPlayer();
	if (ins && ins->init())
	{
		return ins;
	}
	return nullptr;
}

LocalPlayer::~LocalPlayer()
{
}

bool LocalPlayer::init()
{
	return false;
}
