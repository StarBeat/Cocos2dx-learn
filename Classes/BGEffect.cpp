#include "BGEffect.h"
#include "Resources\shader\bg_shader.glsl"

using namespace cocos2d;

BGEffect* BGEffect::create(int id)
{
	auto ins = new BGEffect();
	if (ins && ins->init(id))
	{
		ins->autorelease();
	}
	return ins;
}

void BGEffect::setTexturei(int i, cocos2d::Texture2D* tex)
{
	switch (i)
	{
	case 1:
	{
		_t1 = tex;
		break;
	}
	case 2:
	{
		_t2 = tex;
		break;
	}
	case 3:
	{
		_t3 = tex;
		break;
	}
	case 4:
	{
		_t4 = tex;
		break;
	}
	default:
		abort();
		break;
	}
}

void BGEffect::setTexturei(int i, const std::string& texname)
{
	Texture2D* tex = _director->getTextureCache()->addImage(texname);

	setTexturei(i, tex);
}

#define SP_shadertoy 0

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
#if(SP_shadertoy)

#endif
}

void BGEffect::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_rendercmd.init(RenderQueue::QUEUE_GROUP::OPAQUE_3D);
	_rendercmd.func = std::bind([this] (cocos2d::Mat4 transform)
			{
				auto gl = _eff->getGLProgramState();
				auto  _director = Director::getInstance();
				auto size = Director::getInstance()->getWinSize();
				glBindVertexArray(_vao);
#if(SP_shadertoy)
				gl->setUniformVec3("iResolution", Vec3(size.width, size.height, 1));
				gl->setUniformFloat("iTime", _director->getTotalFrames() * _director->getAnimationInterval());
				gl->setUniformFloat("iTimeDelta", _director->getDeltaTime());
				gl->setUniformInt("iFrame", iframe++);
				if (_t1 != nullptr)
				{
					gl->setUniformTexture("iChannel0", _t1);
				}
				if (_t2 != nullptr)
				{
					gl->setUniformTexture("iChannel1", _t2);

				}
				if (_t3 != nullptr)
				{
					gl->setUniformTexture("iChannel2", _t3);
				}
				if (_t4 != nullptr)
				{
					gl->setUniformTexture("iChannel3", _t4);
				}
#else
				gl->setUniformVec2("resolution", size);
				gl->setUniformFloat("time", _director->getTotalFrames() * _director->getAnimationInterval());

#endif // SP_shadertoy

				gl->apply(transform);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
		}, transform);
	renderer->addCommand(&_rendercmd);
}

#define Name(prefix,n) prefix##n
bool BGEffect::init(int id)
{
	iframe = 0;
	_t1 = _t2 = _t3 = _t4 = nullptr;
	std::string vs = bg_shader_vs;
	std::string fs = Name(bg_shader_fs, 0);
	if (id != 0)
	{
		switch (id)
		{
		case 1:
			{
				fs = Name(bg_shader_fs, 1);
				break;
			}
		case 2:
			{
				fs = Name(bg_shader_fs, 2);
				break;
			}
		case 3:
		{
			fs = Name(bg_shader_fs, 3);
			break;
		}
		case 4:
		{
			fs = Name(bg_shader_fs, 4);
			break;
		}
		case 5:
		{
			fs = Name(bg_shader_fs, 5);
			break;
		}
		case 6:
		{
			fs = Name(bg_shader_fs, 6);
			break;
		}
		case 7:
		{
			fs = Name(bg_shader_fs, 7)[0];
			fs+= Name(bg_shader_fs, 7)[1];
			break;
		}
		case 8:
		{
			fs = Name(bg_shader_fs, 8);
			break;
		}
		case 9:
		{
			fs = Name(bg_shader_fs, 9);
			break;
		}
		}
	}
	_eff = new Effect();
	auto e = EventListenerMouse::create();
#if(SP_shadertoy)
	Vec4 mouse;
	e->onMouseMove = [this, &mouse](EventMouse* event) {
		mouse.x = event->getCursorX();
		mouse.y = event->getCursorY();
		_eff->getGLProgramState()->setUniformVec4("iMouse", mouse);
	};
	e->onMouseDown = [this, &mouse] (EventMouse* event)
	{
		mouse.z = event->getCursorX();
		mouse.w = event->getCursorY();
		//_eff->getGLProgramState()->setUniformVec4("iMouse", mouse);
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(e, this);
	fs = "\nuniform vec3      iResolution;\
			uniform float     iTime;\
			uniform float     iTimeDelta;\
			uniform vec4      iMouse;\
			uniform int       iFrame;  \
			uniform sampler2D iChannel0; \
			uniform sampler2D iChannel1; \
			uniform sampler2D iChannel2; \
			uniform sampler2D iChannel3; \
		 "
		+ fs;
			
#else
	fs += "\nvoid main() \
			{ \
			mainImage(gl_FragColor, gl_FragCoord.xy); \n\
			}";
	auto size = Director::getInstance()->getVisibleSize();
	e->onMouseMove = [this, size](EventMouse* event) {
		_eff->getGLProgramState()->setUniformVec2("mouse", { event->getCursorX()/ size.width , event->getCursorY()/ size .height});
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(e, this);
#endif
	return _eff->initGLProgramState(vs, fs);
}
