#pragma once
#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include <GLESDebugDraw.h>
#include "GameManager.h"

class B2Physic : public cocos2d::Node, public b2ContactListener
{
public:
	static const int PTM_RATIO = 32;
	static const unsigned int UD_TAG = 1 << 32 | 1;
public:
	B2Physic()
	{
	}
	~B2Physic()
	{
		if (GameManager::Instane()->_b2physic == this)
		{
			GameManager::Instane()->_b2physic = nullptr;//析构顺序 导致_listener访问问题
		}
	}
	CREATE_FUNC(B2Physic);

	virtual void BeginContact(b2Contact* contact)
	{
		for (const auto& i : _listener)
		{

			i->BeginContact(contact);
		}
	}

	virtual void EndContact(b2Contact* contact)
	{
		for (const auto& i : _listener)
		{

			i->EndContact(contact);
		}
	}

	virtual void EndContact(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 index)
	{
		for (const auto& i : _listener)
		{

			i->EndContact(fixture, particleSystem, index);
		}
	}

	virtual void EndContact(b2ParticleSystem* particleSystem, int32 indexA, int32 indexB)
	{
		for (const auto& i : _listener)
		{

			i->EndContact(particleSystem, indexA, indexB);
		}
	}

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		for (const auto& i : _listener)
		{

			i->PreSolve(contact, oldManifold);
		}
	}

	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		for (const auto& i : _listener)
		{
			i->PostSolve(contact, impulse);
		}
	}

	void BeginContact(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact)override
	{
		//const_cast<b2ParticleTriad*>(particleSystem->GetTriads())[particleBodyContact->index].flags = b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle;
		//particleSystem->SetParticleFlags(particleBodyContact->index,
		//	b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle
		///* ~b2_elasticParticle ^ b2_destructionListenerParticle ^ b2_zombieParticle ^ b2_fixtureContactListenerParticle ^ b2_particleContactListenerParticle*/);
		for (const auto& i : _listener)
		{

			i->BeginContact(particleSystem, particleBodyContact);
		}
	}
	virtual void BeginContact(b2ParticleSystem* particleSystem, b2ParticleContact* particleContact) override
	{
		/*	const_cast<b2ParticleTriad*>(particleSystem->GetTriads())[particleContact->GetIndexA()].flags = b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle;
			particleSystem->SetParticleFlags(particleContact->GetIndexA(),
				b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle);
			const_cast<b2ParticleTriad*>(particleSystem->GetTriads())[particleContact->GetIndexB()].flags = b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle;
			particleSystem->SetParticleFlags(particleContact->GetIndexB(),
				b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle);*/
		for (const auto& i : _listener)
		{
			i->BeginContact(particleSystem, particleContact);
		}
	}

	void setDebugFlag(uint32 flags)
	{
		_debugDraw->SetFlags(flags);
	}
	bool init()
	{
		b2Vec2 gravity(0.0f, -10.0f);
		_world = new  b2World(gravity);
		_world->SetAllowSleeping(true);
		_world->SetContinuousPhysics(true);
		_debugDraw = new GLESDebugDraw(PTM_RATIO);
		_world->SetDebugDraw(_debugDraw);
		uint32 flags = 0;
		//flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_aabbBit;
		//flags += b2Draw::e_pairBit;
		//flags += b2Draw::e_centerOfMassBit;
		//flags += b2Draw::e_particleBit;
		_debugDraw->SetFlags(flags);
		cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, true);

		_world->SetContactListener(this);
		return true;
	}

	void update(float dt)
	{
		const int velocityIterations = 8;
		const int positionIterations = 1;
		_world->Step(dt, velocityIterations, positionIterations);
	}

	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t transformFlags)
	{
		_customCmd.init(_globalZOrder);
		_customCmd.func = CC_CALLBACK_0(B2Physic::onDraw, this, transform, transformFlags);
		renderer->addCommand(&_customCmd);
	}

	void onDraw(const cocos2d::Mat4& transform, uint32_t transformFlags)
	{
		cocos2d::kmGLPushMatrix();
		cocos2d::kmGLLoadMatrix(&transform);

		cocos2d::GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
		_world->DrawDebugData();

		CHECK_GL_ERROR_DEBUG();

		cocos2d::kmGLPopMatrix();
	}

	void addB2ContactListener(b2ContactListener*& l)
	{
		_listener.push_back(l);
	}

	void removeB2ContactListener(b2ContactListener*& l)
	{
		_listener.remove(l);
	}
private:
	b2World* _world;

	std::list<b2ContactListener*> _listener;

	GLESDebugDraw* _debugDraw;
	cocos2d::CustomCommand _customCmd;
	friend class GameManager;
};