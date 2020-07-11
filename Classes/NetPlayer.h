#pragma once
#include "IPlayer.h"
#include "PlayerManager.h"

class NetPlayer : public IPlayer
{
public:
	static NetPlayer* create(cocos2d::Vec2 pos, cocos2d::Vec2 rot, std::string name);
	void move(float x, float y) override;
	void death() override;
	void respwan() override;
	void respwanNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot);
	~NetPlayer();
protected:
	virtual bool init();
private:
	NetPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot, std::string name);
};

