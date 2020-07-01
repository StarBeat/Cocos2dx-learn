#include "BGEffect.h"
#include "Resources\shader\bg_shader.glsl"
using namespace cocos2d;

BGEffect* BGEffect::create()
{
	auto ins = new BGEffect();
	if (ins && ins->init())
	{
		ins->autorelease();
	}
	return ins;
}

void BGEffect::genBindBuffer()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	Vec2 vertex[] = {{-1.0f,  1.0f},
					 {-1.0f, -1.0f},
					 {1.0f, 1.0f},
					 {1.0f, -1.0f} };

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * 4, vertex, GL_STREAM_DRAW);
	// vertex
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (GLvoid*)(0));
	glBindVertexArray(0);
}

void BGEffect::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_rendercmd.init(RenderQueue::QUEUE_GROUP::OPAQUE_3D);
	_rendercmd.func = std::bind([this] (cocos2d::Mat4 transform)
			{
				auto gl = _eff->getGLProgramState();
				auto  _director = Director::getInstance();
				glBindVertexArray(_vao);
				gl->setUniformVec2("resolution", Director::getInstance()->getWinSize());
				gl->setUniformFloat("time", _director->getTotalFrames() * _director->getAnimationInterval());
				gl->apply(transform);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
				CHECK_GL_ERROR_DEBUG();
		}, transform);
	renderer->addCommand(&_rendercmd);
}

bool BGEffect::init()
{
	std::string vs = bg_shader_vs;
	std::string fs = bg_shader_fs;
	_eff = new Effect();
	return _eff->initGLProgramState(vs, fs);
}
