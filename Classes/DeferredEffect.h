#pragma once
#include "Effect.h"
class DeferredEffect : public Effect
{
public:
	static DeferredEffect* create();
	void genBuffer();
	void draw(GLuint lightmap);
	void reafBuffer();
private:
	bool init();
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _fbo;
	GLuint _colorbuffer;
	GLuint _ebo;
	float _exposureLimit = -1;
	cocos2d::Vec3 _globalLight = cocos2d::Vec3::ZERO;
};

