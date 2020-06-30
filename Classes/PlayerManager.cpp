#include "PlayerManager.h"
#include "GameManager.h"
#include "NetPlayer.h"
#include "LocalPlayer.h"
#include "GameScene.h"
using namespace cocos2d;

template<>
PlayerManager* ISingle<PlayerManager>::instance = nullptr;

PlayerManager::PlayerManager()
{
	_connected = false;
}

void PlayerManager::bindFuncs(int id)
{
	CCLOG("bind net id: (%d)\n", id);

	_rpc = GameManager::Instane()->_network->_rpc;
	if (_rpc->isServer() && _connected)
	{
		_rpc->single_call(id, "setSeed", GameManager::Instane()->_seed);
	}

	if (!_connected)
	{
#pragma region DefultBindpoint
		_rpc->bind(Util::Rpc::DEFAULT_BINDID, "createNetPlayer", &PlayerManager::createNetPlayer, this);
#pragma endregion
#pragma region SelfBindPoint
		_selfid = id;
		_rpc->bind(id,"setSeed",&GameManager::setSeed, GameManager::Instane());
#pragma endregion
		_connected = true;
		createLocalPlayer(Vec2(10, 10) * id, Vec2(0, 0));
	}
}


void PlayerManager::createNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot, int id)
{
	CCLOG("createNetPlayer At (%f,%f)\n", pos.x, pos.y);
	IPlayer* np;
	if (!_npool.empty())
	{
		np = _npool.top();
		_npool.pop();
		np->setPosition(pos);
		np->setRotation(rot);
	}
	else
	{
		np = NetPlayer::create(pos, rot);
	}
	auto gs = GameScene::gameScene;
	auto node = np->operator IPrimitive * ();
	if (!node->isVisible())// false don`t addchild
	{
		node->setVisible(true);
		node->getPhysicsBody()->setEnabled(true);
	}
	else
	{
		gs->addChild(node);
	}
#pragma region NidBindPoint
	_rpc->bind( id, "move", &NetPlayer::move, (NetPlayer*)np);
	_rpc->bind(id, "respwanNetPlayer", &PlayerManager::respwanNetPlayer, this);
#pragma endregion
}

IPlayer* PlayerManager::createLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	_rpc->call(Util::Rpc::DEFAULT_BINDID, "createNetPlayer", pos, rot, _selfid);
	auto gs = GameScene::gameScene;
	IPlayer* lp;
	if (!_lpool.empty())
	{
		lp = _lpool.top();
		_lpool.pop();
		lp->setPosition(pos);
		lp->setRotation(rot);
	}
	else
	{
		lp = LocalPlayer::create(pos, rot);
		auto e = EventListenerKeyboard::create();
		e->onKeyPressed = [lp](EventKeyboard::KeyCode k, Event* e)
		{
			float x = 0, y = 0;
			if (!lp->operator IPrimitive *()->isVisible())
			{
				return;
			}
			switch (k)
			{
				case cocos2d::EventKeyboard::KeyCode::KEY_W:
					y = 10;
					break;
				case cocos2d::EventKeyboard::KeyCode::KEY_S:
					y = -10;
					break;
				case cocos2d::EventKeyboard::KeyCode::KEY_A:
					x = -10;
					break;
				case cocos2d::EventKeyboard::KeyCode::KEY_D:
					x = 10;
					break;
			}
			if (x!= 0 || y !=0)
			{
				((LocalPlayer*)lp)->move(x, y);
			}
		};
		gs->getEventDispatcher()->addEventListenerWithSceneGraphPriority(e, gs);
	}
	auto node = lp->operator IPrimitive * ();
	if (!node->isVisible())// false don`t addchild
	{
		node->setVisible(true);
		node->getPhysicsBody()->setEnabled(true);
	}
	else
	{
		gs->addChild(node);
	}
	return lp;
}

void PlayerManager::respwanNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	IPlayer* np;
	np = _npool.top();
	_npool.pop();
	np->setPosition(pos);
	np->setRotation(rot);
	np->rewpan();
	auto node = np->operator IPrimitive * ();
	node->setVisible(true);
	node->getPhysicsBody()->setEnabled(true);
}

IPlayer* PlayerManager::respwanLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	_rpc->call(_selfid, "respwanNetPlayer", pos, rot);
	IPlayer* lp;
	lp = _lpool.top();
	_lpool.pop();
	lp->setPosition(pos);
	lp->setRotation(rot);
	lp->rewpan();
	auto node = lp->operator IPrimitive * ();
	node->setVisible(true);
	node->getPhysicsBody()->setEnabled(true);
	return lp;
}

void PlayerManager::RecycleLPlayer(LocalPlayer* lp)
{
	lp->operator IPrimitive* ()->stopAllActions();
	lp->operator IPrimitive* ()->setVisible(false);
	lp->operator IPrimitive* ()->getPhysicsBody()->setEnabled(false);
	_lpool.push(lp);
}

void PlayerManager::RecycleNPlayer(NetPlayer* np)
{
	np->operator IPrimitive* ()->stopAllActions();
	np->operator IPrimitive* ()->setVisible(false);
	np->operator IPrimitive* ()->getPhysicsBody()->setEnabled(false);
	_npool.push(np);
}
