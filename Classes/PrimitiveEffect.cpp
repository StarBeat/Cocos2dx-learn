#include "PrimitiveEffect.h"
#include "Resources\shader\primitive_shader.glsl"
using namespace cocos2d;

PrimitiveEffect* PrimitiveEffect::create()
{
	auto ins = new PrimitiveEffect();
	if (ins && ins->init())
	{
		ins->retain();
		return ins;
	}
	return nullptr;
}

void PrimitiveEffect::genBindBuffer(void* buffer, int size)
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F) * size, buffer, GL_STREAM_DRAW);
	// vertex
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid*)offsetof(V2F_C4B_T2F, vertices));
	// color
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F), (GLvoid*)offsetof(V2F_C4B_T2F, colors));
	// texcood
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid*)offsetof(V2F_C4B_T2F, texCoords));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PrimitiveEffect::draw(int vercount)
{
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, vercount);
	glBindVertexArray(0);
	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, vercount);
//	CHECK_GL_ERROR_DEBUG();
}

void PrimitiveEffect::use(const cocos2d::Mat4& mv)
{
	_glprogramstate->apply(mv);
}

bool PrimitiveEffect::init()
{
	std::string vs = primitive_vs;
	std::string fs = primitive_fs;
	return initGLProgramState(vs, fs);
}

PrimitiveEffect::~PrimitiveEffect()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}
