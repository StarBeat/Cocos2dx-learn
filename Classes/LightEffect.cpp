#include "LightEffect.h"
#include "Resources\shader\light2d_shader.glsl"
#include "Light2d.h"

using namespace cocos2d;

GLuint LightEffect::_fbo = -1;
GLuint LightEffect::_lightmap = -1;
bool LightEffect::_start_renderlights = false;

LightEffect* LightEffect::create(Light2d* l)
{
	auto ins = new LightEffect();
	if (ins && ins->init())
	{
		ins->retain();
	}
	ins->_light = l;
	return ins;
}

void LightEffect::genBuffer()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F) * 4, NULL, GL_STREAM_DRAW);
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

	auto wsize = Director::getInstance()->getWinSize();
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glGenTextures(1, &_lightmap);
	glBindTexture(GL_TEXTURE_2D, _lightmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wsize.width, wsize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _lightmap, 0);
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//auto i = _glprogramstate->getGLProgram()->getUniformLocation("_ShadowMap");
	//glUniform1i(i, 0);
}

void LightEffect::bindBufferData()
{
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	buffer[0] = { Vec2(-_lightDistance, _lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {0,1} };
	buffer[1] = { Vec2(-_lightDistance, -_lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {0, 0} };
	buffer[2] = { Vec2(_lightDistance, _lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {1, 1} };
	buffer[3] = { Vec2(_lightDistance, -_lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {1, 0} };


	//buffer[0] = { Vec2(-_lightDistance, -_lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {0,0} };
	//buffer[1] = { Vec2(-_lightDistance, _lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {0, 1} };
	//buffer[2] = { Vec2(_lightDistance, -_lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {1, 0} };
	//buffer[3] = { Vec2(-_lightDistance, _lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {0, 1} };
	//buffer[4] = { Vec2(_lightDistance, _lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {1,1} };
	//buffer[5] = { Vec2(_lightDistance, -_lightDistance),Color4B(_color.r, _color.g, _color.b, _color.a), {1, 0} };

	glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F) * 4, buffer, GL_STREAM_DRAW);
}

void LightEffect::draw(GLuint shadowmap)
{
	bindBufferData();
	_glprogramstate->setUniformFloat("_Attenuation", _attenuation);
	_glprogramstate->setUniformFloat("_Intensity", _intensity);
	_glprogramstate->setUniformVec4("_Color", { _color.r, _color.g, _color.b, _color.a });
	_glprogramstate->setUniformTexture("_ShadowMap", shadowmap);
	_glprogramstate->applyUniforms();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	CHECK_GL_ERROR_DEBUG();
}

void LightEffect::use(const cocos2d::Mat4& mat)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);//ZWrite Off
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);//BlendOp Add
	glBlendFunc(GL_ONE, GL_ONE);//Blend One One
	_glprogramstate->apply(mat);
}

bool LightEffect::init()
{
	std::string vs = light2d_analytic_vs;
	std::string fs = light2d_analytic_fs;
	return initGLProgramState(vs, fs);
}