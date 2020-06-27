#pragma once
#include "Effect.h"
#include "cocos2d.h"

class PrimitiveEffect : public Effect
{
public:
	static PrimitiveEffect* create();
	void genBindBuffer(void* buffer, int size);
	void draw(int vercount);
	void use(const cocos2d::Mat4&);
private:
	bool init();
	PrimitiveEffect() = default;
	~PrimitiveEffect();
	
private:
	GLuint _vao;
	GLuint _vbo;
};

