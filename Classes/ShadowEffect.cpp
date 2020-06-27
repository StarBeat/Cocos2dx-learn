#include "ShadowEffect.h"
#include "Resources\shader\shadow_shader.glsl"
#include "Light2d.h"
using SS_Vertice = Light2d::SS_Vertice;

using namespace cocos2d;

ShadowEffect* ShadowEffect::create(Light2d* l)
{
	auto ins = new ShadowEffect();
	if (ins && ins->init())
	{
		ins->retain();
	}
	ins->_light = l;
	return ins;
}

void ShadowEffect::genBuffer()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	// vertex
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(SS_Vertice), (GLvoid*)offsetof(SS_Vertice, vertices));
	// color
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SS_Vertice), (GLvoid*)offsetof(SS_Vertice, colors));
	// texcood
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(SS_Vertice), (GLvoid*)offsetof(SS_Vertice, uv0));

	//uv2
	auto uv2 = _glprogramstate->getGLProgram()->getAttribLocation("_texCoord2");
	glEnableVertexAttribArray(uv2);
	glVertexAttribPointer(uv2, 2, GL_FLOAT, GL_FALSE, sizeof(SS_Vertice), (GLvoid*)offsetof(SS_Vertice, uv2));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	auto wsize = Director::getInstance()->getWinSize();
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glGenTextures(1, &_shadowmap);
	glBindTexture(GL_TEXTURE_2D, _shadowmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wsize.width, wsize.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _shadowmap, 0);
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowEffect::bindBufferData(void* buffer, int size)
{
	glBindVertexArray(_vao);
	if (size > _lastsize)
	{
		_lastsize = size;
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SS_Vertice) * size, buffer, GL_STREAM_DRAW);
}

void ShadowEffect::draw(int vercount)
{
	glBindVertexArray(_vao);
	//_glprogramstate->setUniformFloat("_LightSize", 1.4);//_lightVolumn
	_glprogramstate->setUniformFloat("_LightVolume", _lightVolumne);//_lightVolumn
	_glprogramstate->applyUniforms();
	glDrawArrays(GL_TRIANGLES, 0,vercount);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERROR_DEBUG();
}

void ShadowEffect::use(const cocos2d::Mat4& mv)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_glprogramstate->apply(mv);
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);//ZWrite Off

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);//BlendOp Add
	glBlendFunc(GL_ONE, GL_ONE);//Blend One One
}

bool ShadowEffect::init()
{
	std::string vs = shadow_vs;
	std::string fs = shadow_fs;
	return initGLProgramState(vs, fs);
}
