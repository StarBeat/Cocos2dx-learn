#pragma once
#include "IPlayer.h"
class LocalPlayer : public IPlayer
{
public:
	LocalPlayer* create(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
	virtual ~LocalPlayer();
protected:
	virtual bool init();
private:
	LocalPlayer() = default;
};

