#pragma once
#include <cocos2d.h>
#include <Box2D\Box2D.h>
#include <GLESDebugDraw.h>


class B2Physic : public cocos2d::Node
{
public:
	static const int PTM_RATIO = 32;
public:
	B2Physic() {}
	~B2Physic() {}
	CREATE_FUNC(B2Physic);

	bool init()
	{
		b2Vec2 gravity(0.0f, -10.0f);
		_world = new  b2World(gravity);
		_world->SetAllowSleeping(true);
		_world->SetContinuousPhysics(true);
#if _DEBUG
		_debugDraw = new GLESDebugDraw(PTM_RATIO);
		_world->SetDebugDraw(_debugDraw);
		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_jointBit;
		flags += b2Draw::e_aabbBit;
		flags += b2Draw::e_pairBit;
		flags += b2Draw::e_centerOfMassBit;
		flags += b2Draw::e_particleBit;
		_debugDraw->SetFlags(flags);
#endif
		cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
		struct LS : b2ContactListener
		{
			void BeginContact(b2Contact* contact) override
			{
				auto i = contact->GetChildIndexA();
				auto j = contact->GetChildIndexB();

			}
			void BeginContact(b2ParticleSystem* particleSystem,
				b2ParticleBodyContact* particleBodyContact)override
			{
				const_cast<b2ParticleTriad*>(particleSystem->GetTriads())[particleBodyContact->index].flags = b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle;
				particleSystem->SetParticleFlags(particleBodyContact->index,
					b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle
				/* ~b2_elasticParticle ^ b2_destructionListenerParticle ^ b2_zombieParticle ^ b2_fixtureContactListenerParticle ^ b2_particleContactListenerParticle*/);
			}
			virtual void BeginContact(b2ParticleSystem* particleSystem,
				b2ParticleContact* particleContact) override
			{
				const_cast<b2ParticleTriad*>(particleSystem->GetTriads())[particleContact->GetIndexA()].flags = b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle;
				particleSystem->SetParticleFlags(particleContact->GetIndexA(),
					b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle);
				const_cast<b2ParticleTriad*>(particleSystem->GetTriads())[particleContact->GetIndexB()].flags = b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle;
				particleSystem->SetParticleFlags(particleContact->GetIndexB(),
					b2_waterParticle | b2_fixtureContactListenerParticle | b2_particleContactListenerParticle);
			}
		};
	/*	auto listener = new LS();
		_world->SetContactListener(listener);*/
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
private:
	b2World* _world;
#if _DEBUG
	GLESDebugDraw* _debugDraw;
	cocos2d::CustomCommand _customCmd;
#endif // _DEBUG
	friend class GameManager;
};