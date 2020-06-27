#pragma once
#include "Effect.h"
#include "Light2d.h"

class ShadowEffect : public Effect
{
public:
	static ShadowEffect* create(Light2d* l);
	void genBuffer();
	void bindBufferData(void* buffer, int size);
	void draw(int vercount);
	void use(const cocos2d::Mat4&);
	GLuint getShadowMap()
	{
		return _shadowmap;
	}
private:
	ShadowEffect() {}
	bool init();
	float _lightVolumne = 140;
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _fbo;
	GLuint _shadowmap;
	Light2d* _light;
	int _lastsize;
	friend class Light2d;
};

