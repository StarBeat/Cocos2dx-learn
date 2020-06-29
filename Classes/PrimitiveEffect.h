#pragma once
#include "Effect.h"
#include "cocos2d.h"

class PrimitiveEffect : public Effect
{
public:
	static PrimitiveEffect* create();
	void genBindBuffer();
	void bindBufferData(void* vbuffer, int vsize);
	void bindMatBufferData(void* mbuffer, int msize);
	void draw(int vertcount, int objcount);
	void use();
private:
	bool init();
	PrimitiveEffect() = default;
	~PrimitiveEffect();
	
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _matbuffer;
	static cocos2d::GLProgramState* _glprogramstate;
};

