#pragma once
#include "Effect.h"
class Light2d;
class LightEffect : public Effect
{
public:
	static LightEffect* create(Light2d* l);
	void genBuffer();
	void bindBufferData();
	void draw(GLuint shadowmap);
	void use(const cocos2d::Mat4& mat);
	static void lightRenderStart()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		if (!_start_renderlights)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_start_renderlights = true;
	}
	static void lightRenderEnd()
	{
		_start_renderlights = false;
	}
	static GLuint getLightMap()
	{
		return _lightmap;
	}
protected:
	bool init();
	cocos2d::Color4F _color = cocos2d::Color4F::WHITE;
	float _attenuation = 0.5;
	float _intensity = 1;
	float _lightDistance = 480;
	int _smoothRadius = 100;
	float _lightVolumn = 140;
private:
	LightEffect() {}
	GLuint _vao;
	GLuint _vbo;
	cocos2d::V2F_C4B_T2F buffer[6];
	static GLuint _fbo;
	static GLuint _lightmap;
	static bool _start_renderlights;
	Light2d* _light;
	friend class Light2d;
};

