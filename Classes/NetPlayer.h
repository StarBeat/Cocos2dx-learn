#pragma once
#include "IPlayer.h"
class NetPlayer : public IPlayer
{
public:
	static NetPlayer* create(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
	void move(float x, float y) override;
	~NetPlayer();
protected:
	virtual bool init();
private:
	NetPlayer(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
};

