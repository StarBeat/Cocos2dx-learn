#pragma once

#include "Effect.h"
#include "cocos2d.h"
#include <Box2D\Box2D.h>

using cocos2d::Color3B;
struct Particle;

class ParticleEffectSpawn : public cocos2d::Node
{
public:
	enum Type
	{
		WATER,
		GAS,
		LAVA,
		MAX
	}_type;
	static Color3B colors[MAX];

	struct  LS : public b2ContactListener
	{
		virtual void BeginContact(b2Contact* contact);
	};
	float GAS_FLOATABILITY = 7.0f;
	float SPAWN_INTERVAL = 0.025f;
	float timeAcumular = 0;
	float particleLifeTime = 3.0f;
	//void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	void visit(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;

	void addNewSpriteAtPosition(cocos2d::Vec2 p);

	ParticleEffectSpawn();
	~ParticleEffectSpawn();

	void setType(Type t);
	void update(float dt);
	bool init();

	CREATE_FUNC(ParticleEffectSpawn);
private:
	b2ParticleSystem* _particleSystem;
	LS* listener;
	cocos2d::Sprite* _particleSp;
	std::queue<Particle*> _pool;
	std::list<Particle*> _children;
	GLuint _vao;
	GLuint _vbo;
	GLuint _fbo;
	GLuint _tempfbo;
	GLuint _colorbuffer;
	GLuint _deepbuffer;
	GLuint _particlebuffer;
	GLuint _ebo;
	cocos2d::GLProgramState* _target;
	float scale = 0.15f;



	friend class Particle;
};