#include "LightingManager.h"
#include "Light2d.h"

void LightingManager::init()
{
	deferred == nullptr;
}

void LightingManager::pushMat(Light2d* light, const cocos2d::Mat4& mat)
{
	lights[light] = mat;
	++_mats;
	if (_mats == lights.size())
	{
		draw();
	}
}

LightingManager::LightingManager()
{

}

void LightingManager::draw()
{
	if (deferred == nullptr)
	{
		deferred = DeferredRender::create();
		deferred->genBuffer();
	}
	_mats = 0;
	deferred->reafBuffer();
	LightEffect::lightRenderStart();
	for (auto l : lights)
	{
		l.first->drawShadowAndLight(l.second);
	}
	LightEffect::lightRenderEnd();
	deferred->draw(LightEffect::getLightMap());
	//deferred light

}
