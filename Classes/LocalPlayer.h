#pragma once
#include "IPlayer.h"
class LocalPlayer : public IPlayer
{
public:
	static LocalPlayer* create(const cocos2d::Vec2& pos,const cocos2d::Vec2& rot);
	void move(float x, float y) override;
	void rewpan() override;
	virtual ~LocalPlayer();
protected:
	virtual bool init();
	int _selfid;
private:
	LocalPlayer(const cocos2d::Vec2& pos, const cocos2d::Vec2& rot);
};

