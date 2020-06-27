#pragma once
#include "IPlayer.h"
class NetPlayer : public IPlayer
{
public:
	NetPlayer create(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
	virtual ~NetPlayer();
protected:
	virtual bool init();
private:
	NetPlayer() = default;
};

