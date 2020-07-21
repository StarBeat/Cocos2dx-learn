#include "DeferredRender.h"
#include "Resources\shader\deferred_light_shader.glsl"
#include "Light2d.h"

using namespace cocos2d;

DeferredRender* DeferredRender::create()
{
    auto ins = new DeferredRender();
    ins->init();
    ins->retain();
    return ins;
}

void DeferredRender::genBuffer()
{
	struct Data
	{
		Vec2 pos;
		Vec2 uv;

	}buffer[4]{
		{{-1.0f,  1.0f},{ 0.0f, 1.0f}},
		{{-1.0f, -1.0f}, {0.0f, 0.0f}},
		{{1.0f,  1.0f}, {1.0f, 1.0f}},
		{{1.0f, -1.0f}, {1.0f, 0.0f}}
	};
	int index[6] = { 0, 1, 2,
					 1, 3, 2};//还有三角形环绕顺序要求？

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Data) * 4, buffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	// vertex
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Data), (GLvoid*)offsetof(Data, pos));
	// texcood
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Data), (GLvoid*)offsetof(Data, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	auto wsize = Director::getInstance()->getWinSize();
	glGenTextures(1, &_colorbuffer);
	glBindTexture(GL_TEXTURE_2D, _colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wsize.width, wsize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorbuffer, 0);
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};

	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wsize.width, wsize.height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glDrawBuffers(1, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRender::draw(GLuint lightmap)
{
	//_globalLight = Vec3(0.1, 0.1, 0);
	_glprogramstate->getGLProgram()->use();
	_glprogramstate->setUniformFloat("_ExposureLimit", _exposureLimit);
	_glprogramstate->setUniformVec3("_GlobalLight", _globalLight);
	_glprogramstate->setUniformTexture("_LightMap", lightmap);
	_glprogramstate->setUniformTexture("_ColorMap", _colorbuffer);
	_glprogramstate->applyUniforms();
	glBindVertexArray(_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
	glBindVertexArray(0);
	CHECK_GL_ERROR_DEBUG();
}

void DeferredRender::reafBuffer()
{
	auto wsize = Director::getInstance()->getWinSize();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo); // write to default framebuffer
	glBlitFramebuffer(0, 0, wsize.width, wsize.height, 0, 0, wsize.width, wsize.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool DeferredRender::init()
{
    std::string vs = deferred_light_vs;
    std::string fs = deferred_light_fs;
    return initGLProgramState(vs, fs);
}
