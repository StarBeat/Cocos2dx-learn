#pragma once
#include "Effect.h"

class BGEffect : public cocos2d::Node
{
public:
	static BGEffect* create(int id = 0);
	void setTexturei(int i, cocos2d::Texture2D* tex);
	void setTexturei(int i, const std::string& texname);
	void genBindBuffer();
	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	bool init(int id);
private:
	cocos2d::CustomCommand _rendercmd;
	Effect* _eff;
	unsigned int _vao;
	unsigned int _vbo;
	int iframe;
	cocos2d::Texture2D* _t1;
	cocos2d::Texture2D* _t2;
	cocos2d::Texture2D* _t3;
	cocos2d::Texture2D* _t4;
};