#include "PrimitiveEffect.h"
#include "Resources\shader\primitive_shader.glsl"
using namespace cocos2d;

GLProgramState* PrimitiveEffect::_glprogramstate = nullptr;
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

void PrimitiveEffect::genBindBuffer()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	// vertex
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid*)offsetof(V2F_C4B_T2F, vertices));
	// color
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F), (GLvoid*)offsetof(V2F_C4B_T2F, colors));
	// texcood
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid*)offsetof(V2F_C4B_T2F, texCoords));

	glGenBuffers(1, &_matbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _matbuffer);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (GLvoid*)(0));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 16, (GLvoid*)(4 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (GLvoid*)(8 * sizeof(float)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (GLvoid*)(12 * sizeof(float)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glBindVertexArray(0);
}

void PrimitiveEffect::bindBufferData(void* vbuffer, int vsize)
{
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F) * vsize, vbuffer, GL_STREAM_DRAW);

	glBindVertexArray(0);
}

void PrimitiveEffect::bindMatBufferData(void* mbuffer, int msize)
{
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _matbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * msize, mbuffer, GL_STREAM_DRAW);
	glBindVertexArray(0);
}

void PrimitiveEffect::draw(int vertcount, int objcount)
{
	_glprogramstate->applyAttributes();
	_glprogramstate->applyUniforms();
	glBindVertexArray(_vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertcount, objcount);

	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, vertcount);

	glBindVertexArray(0);
	//CHECK_GL_ERROR_DEBUG();
}

void PrimitiveEffect::use()
{
	_glprogramstate->getGLProgram()->use();
}

bool PrimitiveEffect::init()
{
	if (_glprogramstate == nullptr)
	{
		std::string vs = primitive_vs;
		std::string fs = primitive_fs;
		bool rt = initGLProgramState(vs, fs);
		if (rt)
		{
			_glprogramstate = getGLProgramState();
		}
	}
	Effect::_glprogramstate = this->_glprogramstate;
	return true;
}

PrimitiveEffect::~PrimitiveEffect()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}
