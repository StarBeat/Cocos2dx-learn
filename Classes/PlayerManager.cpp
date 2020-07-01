#include "PlayerManager.h"
#include "GameManager.h"
#include "NetPlayer.h"
#include "LocalPlayer.h"
#include "GameScene.h"
#include "AsteroidManager.h"

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
#pragma region ServerBindPoint
		_selfid = id;
		_rpc->bind(Util::Rpc::SERVERID,"setSeed",&GameManager::setSeed, GameManager::Instane());
#pragma endregion
		_connected = true;
		createLocalPlayer(Vec2(100, 100) * abs(id + 1), Vec2(0, 0));
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
	_players.push_back(np);
#pragma region NidBindPoint
	_rpc->bind( id, "move", &NetPlayer::move, (NetPlayer*)np);
	_rpc->bind(id, "respwanNetPlayer", &PlayerManager::respwanNetPlayer, this);
#pragma endregion
}

IPlayer* PlayerManager::createLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	_rpc->call(Util::Rpc::DEFAULT_BINDID, "createNetPlayer", pos, rot, _selfid);
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
		e->onKeyPressed = std::bind(&PlayerManager::onKeyPressed, this, lp, std::placeholders::_1, std::placeholders::_2);
		e->onKeyReleased = std::bind(&PlayerManager::onKeyReleased, this, std::placeholders::_1, std::placeholders::_2);
		_gamescene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(e, _gamescene);
	}
	_lplayer = static_cast<LocalPlayer*>(lp);
	_players.push_back(lp);
	auto node = lp->operator IPrimitive * ();
	if (!node->isVisible())// false don`t addchild
	{
		node->setVisible(true);
		node->getPhysicsBody()->setEnabled(true);
	}
	else
	{
		_gamescene->addChild(node);
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
	_players.push_back(np);
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
	_players.push_back(lp);
	return lp;
}

void PlayerManager::RecycleLPlayer(LocalPlayer* lp)
{
	_players.remove(lp);
	lp->operator IPrimitive* ()->stopAllActions();
	lp->operator IPrimitive* ()->setVisible(false);
	lp->operator IPrimitive* ()->getPhysicsBody()->setEnabled(false);
	_lpool.push(lp);
}

void PlayerManager::RecycleNPlayer(NetPlayer* np)
{
	_players.remove(np);
	np->operator IPrimitive* ()->stopAllActions();
	np->operator IPrimitive* ()->setVisible(false);
	np->operator IPrimitive* ()->getPhysicsBody()->setEnabled(false);
	_npool.push(np);
}

void PlayerManager::init()
{
	_gamescene = GameScene::gameScene;
	if (_gamescene == nullptr)
	{
		return;
	}
	auto physicev = EventListenerPhysicsContact::create();//EventListenerPhysicsContactWithGroup::create(::PhysicEx::NODE_GROUP::PLAYER_GROUP);
	physicev->onContactBegin = OnColliderEntry;
	physicev->onContactPreSolve = OnColliderPreSolve;
	physicev->onContactPostSolve = OnColliderPostSolve;
	physicev->onContactSeparate = OnColliderSeparate;
	_gamescene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(physicev, _gamescene);
	_gamescene->getScheduler()->scheduleUpdate(this, 0, false);
	
	AsteroidManager::Instane()->init();
}

void PlayerManager::update(float df)
{
	float x = 10 * (-_ka + _kd), y = 10 * (_kw - _ks);
	if (x != 0 || y != 0)
	{
		auto vsize = Director::getInstance()->getVisibleSize();
		auto cam = _gamescene->getDefaultCamera();
		Rect rc(cam->getPosition() - vsize / 2 + Size(100, 100), vsize - Size(200, 200));
		if (!rc.containsPoint(_lplayer->_primitive->getPosition() + Vec2(x, y)))
		{
			cam->setPosition(cam->getPosition() + Vec2(x, y));
		}
		((LocalPlayer*)_lplayer)->move(x, y);
	}
}

bool PlayerManager::OnColliderEntry(PhysicsContact& contact)
{
	auto a = contact.getShapeA()->getBody();
	auto b = contact.getShapeB()->getBody();

	return true;
}

bool PlayerManager::OnColliderPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	auto a = contact.getShapeA()->getBody();
	auto b = contact.getShapeB()->getBody();

	return false;//½ûÖ¹Åö×²
}

void PlayerManager::OnColliderPostSolve(PhysicsContact& contact, const PhysicsContactPostSolve& solve)
{
}

void PlayerManager::OnColliderSeparate(PhysicsContact& contact)
{
	auto a = contact.getShapeA()->getBody();
	auto b = contact.getShapeB()->getBody();

}

void PlayerManager::onKeyPressed(IPlayer* player, EventKeyboard::KeyCode k, Event* e)
{
	auto gs = GameScene::gameScene;
	if (!player->operator IPrimitive * ()->isVisible())
	{
		_kw = false;
		_ks = false;
		_ka = false;
		_kd = false;
		_kspace = false;
		return;
	}
	switch (k)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		_kw = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		_ks = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		_ka = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		_kd = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		_kspace = true;
		break;
	}

}

void PlayerManager::onKeyReleased(EventKeyboard::KeyCode k, Event* e)
{
	switch (k)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		_kw = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		_ks = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		_ka = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		_kd = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		_kspace = false;
		break;
	}
}
