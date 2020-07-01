#pragma once
#include "Effect.h"

class BGEffect : public cocos2d::Node
{
public:
	static BGEffect* create();
	void genBindBuffer();
	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	bool init();
private:
	cocos2d::CustomCommand _rendercmd;
	Effect* _eff;
	unsigned int _vao;
	unsigned int _vbo;
};