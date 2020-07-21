#pragma once
#include "ISingle.h"
#include "cocos2d.h"
#include "DeferredRender.h"

class Light2d;
struct LightingManager : public ISingle<LightingManager>
{
	std::unordered_map<Light2d*, cocos2d::Mat4> lights;
	DeferredRender* deferred = nullptr;
	LightingManager();

	void init();
	void pushMat(Light2d* light,const cocos2d::Mat4& mat);
	void draw();
private:
	int _mats = 0;
};