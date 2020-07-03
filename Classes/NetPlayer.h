#pragma once
#include "IPlayer.h"
class NetPlayer : public IPlayer
{
public:
	static NetPlayer* create(cocos2d::Vec2 pos, cocos2d::Vec2 rot, std::string name);
	void move(float x, float y) override;
	void respwan() override;
	~NetPlayer();
protected:
	virtual bool init();
private:
	NetPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot, std::string name);
};

