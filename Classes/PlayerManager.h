#pragma once
#include "ISingle.h"
#include <stack>
#include "cocos2d.h"
#include "Serializer.h"

using Util::Serializer;
using cocos2d::Vec2;
class LocalPlayer;
class NetPlayer;
class IPlayer;

struct Vec2Ex : Vec2
{
	Vec2Ex() : Vec2(){}
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
	void createNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot);
	IPlayer* createLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot);
	IPlayer* respwanNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot);
	IPlayer* respwanLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot);

	void RecycleLPlayer(LocalPlayer* lp)
	{
		_lpool.push(lp);
	}
	void RecycleNPlayer(NetPlayer* lp)
	{
		_npool.push(lp);
	}
private:
	bool _connected;
	int _selfid;
	std::stack<LocalPlayer*> _lpool;
	std::stack<NetPlayer*> _npool;
	Util::Rpc* _rpc;

	friend class IPlayer;
};

