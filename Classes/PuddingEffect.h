#pragma once

#include "Effect.h"
#include "cocos2d.h"
#include <Box2D\Box2D.h>

class PuddingEffect : public Effect
{
public:
	enum Type
	{
		Box,
		Circle
	};
	PuddingEffect(cocos2d::Texture2D*);
	~PuddingEffect();
	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags);
	void onDraw(const cocos2d::Mat4& transform, uint32_t transformFlags);
	void initData(Type t,cocos2d::Vec2 position, float stride = 0.25);
private:
	bool init();

	b2ParticleSystem* _particleSystem;
	cocos2d::Mat4 _ratioTransform;
	float _ratio;
	GLuint _vao;
	GLuint _vbo;
	GLuint _vbo1;
	GLuint _ebo;
	cocos2d::BlendFunc _blendFunc;
	cocos2d::Texture2D* _texture;
	cocos2d::CustomCommand _customCommand;
	cocos2d::RenderTexture* _renderTexture;
	std::vector<cocos2d::Vec2> texCoords;
	std::vector<int> indexs;
	cocos2d::Size texSize;
};