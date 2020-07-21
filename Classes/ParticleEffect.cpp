#include "ParticleEffect.h"
#include "GameManager.h"
#include "Resources\shader\particle_shader.glsl"
#include <PhysicSprite.h>
USING_NS_CC;
#define PTM_RATIO B2Physic::PTM_RATIO

struct Particle
{
	PhysicSprite* sp;
	ParticleEffectSpawn::Type _t;
	ParticleEffectSpawn* _pe;
	void setType(ParticleEffectSpawn::Type t)
	{
		_t = t;
		b2Body* body = sp->getB2Body();
		sp->stopAllActions();
		sp->setColor(ParticleEffectSpawn::colors[_t]);
		float ts = 1;
		switch (_t)
		{
		case ParticleEffectSpawn::WATER:
			body->SetGravityScale(1.0f);
			body->GetFixtureList()->SetSensor(false);
			break;
		case ParticleEffectSpawn::GAS:
			ts = 0.5f;
			body->SetGravityScale(0.f);
			body->GetFixtureList()->SetSensor(true);
			break;
		case ParticleEffectSpawn::LAVA:
			body->SetGravityScale(0.3f);
			body->GetFixtureList()->SetSensor(false);
			break;
		}
		sp->setScaleX(_pe->scale);
		sp->setScaleY(_pe->scale);
		body->SetUserData((void*)this);
		auto action = ScaleTo::create(_pe->particleLifeTime * ts,0);
		auto seq = Sequence::create(action, CallFunc::create([this]() {
			_pe->_children.remove(this);
			_pe->_pool.push(this);
			sp->getB2Body()->SetActive(false);
			}), NULL);
		sp->runAction(seq);
		sp->resume();
	}
};
Color3B ParticleEffectSpawn::colors[] = { Color3B::BLUE, Color3B::GREEN, Color3B::RED };

ParticleEffectSpawn::ParticleEffectSpawn()
{
	b2ParticleSystemDef particleSystemDef;
	particleSystemDef.dampingStrength = 0.2f;
	particleSystemDef.radius = 0.1f;
	_particleSystem = GameManager::Instane()->_b2world->CreateParticleSystem(&particleSystemDef);
	_particleSystem->SetGravityScale(0.4f);
	_particleSystem->SetDensity(1.2f);
	_scheduler->scheduleUpdate(this, 0, false);
	listener = new LS();
	setType(Type::WATER);
	GameManager::Instane()->_b2physic->addB2ContactListener(listener);
}

ParticleEffectSpawn::~ParticleEffectSpawn()
{
	delete listener;
}

void ParticleEffectSpawn::setType(Type t)
{
	_type = t;
}

void ParticleEffectSpawn::update(float dt)
{
	if (timeAcumular > SPAWN_INTERVAL)
	{
		timeAcumular = 0;
		addNewSpriteAtPosition(getPosition());
	}
	timeAcumular += dt;
	for (const auto& i : _children)
	{
		if (i->_t == GAS)
		{
			auto body = i->sp->getB2Body();
		
			if (body->GetLinearVelocity().y < 50 / PTM_RATIO)
			{
				float m = body->GetMass();
				float t = 1 / 60;
				auto v = b2Vec2(0, GAS_FLOATABILITY) - body->GetLinearVelocity();
				body->ApplyLinearImpulse( m * v, body->GetWorldCenter(), false);
			}
		}
		else
		{
			Vec2 movementScale(i->sp->getScaleX(), i->sp->getScaleY());
			auto body = i->sp->getB2Body();
			movementScale.x += abs(body->GetLinearVelocity().x) / 30.0f;
			movementScale.y += abs(body->GetLinearVelocity().y) / 30.0f;
			i->sp->setScaleX(movementScale.x);
			i->sp->setScaleY(movementScale.y);
		}
	}
}

bool ParticleEffectSpawn::init()
{
	auto s = Director::getInstance()->getWinSize();

	// Change RenderTexture shader
	auto program = GLProgram::createWithByteArrays(particle_shader_vs, particle_shader_fs);
	_target = GLProgramState::getOrCreateWithGLProgram(program);

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
					 1, 3, 2 };//还有三角形环绕顺序要求？

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
	glGenTextures(1, &_particlebuffer);
	glBindTexture(GL_TEXTURE_2D, _particlebuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wsize.width, wsize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _particlebuffer, 0);
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &_tempfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _tempfbo);
	glGenTextures(1, &_colorbuffer); 
	glGenTextures(1, &_deepbuffer); 
	glBindTexture(GL_TEXTURE_2D, _colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wsize.width, wsize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorbuffer, 0);
	glDrawBuffers(1, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void ParticleEffectSpawn::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	renderer->render();
	auto wsize = Director::getInstance()->getWinSize();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _tempfbo);
	glBlitFramebuffer(0, 0, wsize.width, wsize.height, 0, 0, wsize.width, wsize.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& i : _children)
	{
		i->sp->visit(renderer, parentTransform, parentFlags);
	}
	renderer->render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	_target->getGLProgram()->use();
	glBindVertexArray(_vao);
	_target->setUniformTexture("_ParticleColorMap", _particlebuffer);
	_target->setUniformTexture("_ColorMap", _colorbuffer);
	_target->applyUniforms();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
	glBindVertexArray(0);

}

void ParticleEffectSpawn::addNewSpriteAtPosition(Vec2 p)
{
	PhysicSprite* _particleSp;
	b2Body* body;
	Particle* pt;
	if (_pool.empty())
	{
		_particleSp = PhysicSprite::create("ParticleImage.png");
		_particleSp->retain();
		pt = new Particle{ _particleSp, _type, this };
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody; 
		bodyDef.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);
		body = GameManager::Instane()->_b2world->CreateBody(&bodyDef);
		body->SetLinearDamping(0);
		body->SetAngularDamping(0.1f);
		b2CircleShape dynamicCircle;
		dynamicCircle.m_radius = _particleSp->getContentSize().width / 10 / PTM_RATIO * scale;
		// Define the dynamic body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicCircle;
		fixtureDef.density = 1.0f; 
		fixtureDef.friction = 0.0f;
		fixtureDef.restitution = 0; 
		body->CreateFixture(&fixtureDef);
		_particleSp->setB2Body(body);
		_particleSp->setPTMRatio(PTM_RATIO);
	}
	else
	{
		pt = _pool.front();
		pt->_t = _type;
		_particleSp = pt->sp;
		_particleSp->setPosition(p);
		body = _particleSp->getB2Body();
		body->SetTransform({ p.x / PTM_RATIO, p.y / PTM_RATIO }, 0);
		body->SetActive(true);
		body->SetLinearVelocity({ 0,0 });
		body->SetAngularVelocity(0);
		body->ApplyLinearImpulse({ 0,0 }, body->GetWorldCenter(), true);
		body->ApplyForce({ 0,0 }, body->GetWorldCenter(), true);
		_pool.pop();
	}
	_children.push_back(pt);
	_particleSp->setScaleX(scale);
	_particleSp->setScaleY(scale);

	_particleSp->setColor(colors[(int)_type]);
	float ts = 1;
	switch (_type)
	{
	case ParticleEffectSpawn::WATER:
		body->SetGravityScale(1.0f);
		body->GetFixtureList()->SetSensor(false);
		break;
	case ParticleEffectSpawn::GAS://不直接生成
		ts = 0.5f;
		body->SetGravityScale(0.0f);
		body->GetFixtureList()->SetSensor(true);
		break;
	case ParticleEffectSpawn::LAVA:
		body->SetGravityScale(0.3f);
		body->GetFixtureList()->SetSensor(false);
		break;
	}
	body->SetUserData((void*)pt);
	auto action = ScaleTo::create(particleLifeTime * ts,0);
	auto seq = Sequence::create(action, CallFunc::create([pt,this]() {
			_children.remove(pt);
			pt->sp->getB2Body()->SetActive(false);
			_pool.push(pt);
		}), NULL);
	_particleSp->runAction(seq);
	_particleSp->resume();
}

void ParticleEffectSpawn::LS::BeginContact(b2Contact* contact)
{
	auto b1 = contact->GetFixtureA()->GetBody();
	auto b2 = contact->GetFixtureB()->GetBody();
	Particle* p1 = (Particle*)b1->GetUserData();
	Particle* p2 = (Particle*)b2->GetUserData();
	if (p1 && p2 && p1->_t == Type::WATER)
	{
		if (p2->_t == Type::LAVA)
		{
			contact->GetFixtureA()->SetSensor(true);
			p1->setType(Type::GAS);
		}
	}
}
