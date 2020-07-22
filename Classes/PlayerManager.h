#pragma once
#include "ISingle.h"
#include <stack>
#include <list>
#include "cocos2d.h"
#include "include\Serializer.h"

using Util::Serializer;
using cocos2d::Vec2; 
using cocos2d::EventKeyboard;
using cocos2d::Event;
using cocos2d::PhysicsContact;
using cocos2d::PhysicsContactPreSolve;
using cocos2d::PhysicsContactPostSolve;
class LocalPlayer;
class NetPlayer;
class IPlayer;
class GameScene;

struct Vec2Ex : Vec2
{
	Vec2Ex() : Vec2() {}
	Vec2Ex(float xx, float yy) : Vec2(xx, yy){}
	Vec2Ex(const Vec2& v) :Vec2(v){}
	friend Serializer& operator >> (Serializer& in, Vec2Ex& d) {
		in >> d.x >> d.y;
		return in;
	}
	friend Serializer& operator << (Serializer& out, Vec2Ex d) {
		out << d.x << d.y;
		return out;
	}
};

class PlayerManager : public ISingle<PlayerManager>
{
public:
	PlayerManager();
	
	void bindFuncs(int id);

	void createNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot, int id);
	IPlayer* createLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot);
	IPlayer* respwanLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot);

	
	void init();
	void update(float df);
	void stop() { _rpc->disconnect(_selfid); }
	static bool OnColliderEntry(PhysicsContact& contact);
	static bool OnColliderPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve);
	static void OnColliderPostSolve(PhysicsContact& contact, const PhysicsContactPostSolve& solve);
	static void OnColliderSeparate(PhysicsContact& contact);

	void onKeyPressed(IPlayer* player,EventKeyboard::KeyCode k, Event* e);
	void onKeyReleased(EventKeyboard::KeyCode k, Event* e);
	
private:
	bool _connected;
	int _selfid;
	std::list<IPlayer*> _players;
	Util::Rpc* _rpc;
	LocalPlayer* _lplayer;
	GameScene* _gamescene;
#pragma region KeyStay
	bool _kw = false;
	bool _ks = false;
	bool _ka = false;
	bool _kd = false;
	bool _kspace = false;
#pragma endregion

	friend class IPlayer;
	friend class NetPlayer;
	friend class LocalPlayer;
	friend class AsteroidManager;
};

