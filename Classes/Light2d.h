#pragma once
#include "LightEffect.h"
#include "ShadowEffect.h"
#include "cocos2d.h"
#include "LightingManager.h"
#include <InstanceRenderCommand.h>
using cocos2d::V2F_C4B_T2F;

class Light2d : public cocos2d::Node
{
public:
	struct SS_Vertice
	{
		/// vertices (2F)
		cocos2d::Vec2 vertices;
		/// colors (4B)
		cocos2d::Color4B colors;
		/// tex coords (2F)
		cocos2d::Vec2 uv0;
		cocos2d::Vec2 uv2;
	};
public:
	static Light2d* create();
	~Light2d() {}
	void calculateShadow();
	void renderLighting(const cocos2d::Mat4& transform);
	void renderShadow(const cocos2d::Mat4& transform);
	void drawShadowAndLight(const cocos2d::Mat4& transform);
	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
private:
	Light2d();
	void debugDrawline(cocos2d::Vec2 a, cocos2d::Vec2 b,const cocos2d::Color4F&);
private:
	ShadowEffect* _shadoweff;
	LightEffect* _lighteff;
	InstanceRenderCommand _instancecommande;

	std::vector<SS_Vertice> _shadowtrianglesbuffer;
	cocos2d::Color4B _shadow_color = cocos2d::Color4B::BLACK;


	friend class LightingManager;
};

