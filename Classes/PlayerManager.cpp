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
		if (_rpc->isServer())
		{
			AsteroidManager::Instane()->init();
		}
		createLocalPlayer(Vec2(100, 100) * abs(id + 3), Vec2(0, 0));
	}
}


void PlayerManager::createNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot, int id)
{
	CCLOG("createNetPlayer At (%f,%f)\n", pos.x, pos.y);
	IPlayer* np = NetPlayer::create(pos, rot, "Player" + std::to_string(id));
	auto gs = GameScene::gameScene;
	auto node = np->operator IPrimitive * ();
	
	gs->addChild(node);
	_players.push_back(np);
#pragma region NidBindPoint
	_rpc->bind( id, "move", &NetPlayer::move, (NetPlayer*)np);
	_rpc->bind(id, "respwanNetPlayer", &NetPlayer::respwanNetPlayer, (NetPlayer*)np);
#pragma endregion
}

IPlayer* PlayerManager::createLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	_rpc->call(Util::Rpc::DEFAULT_BINDID, "createNetPlayer", pos, rot, _selfid);
	IPlayer* lp = LocalPlayer::create(pos, rot, "Player" + std::to_string(_selfid));
	auto e = EventListenerKeyboard::create();
	e->onKeyPressed = std::bind(&PlayerManager::onKeyPressed, this, lp, std::placeholders::_1, std::placeholders::_2);
	e->onKeyReleased = std::bind(&PlayerManager::onKeyReleased, this, std::placeholders::_1, std::placeholders::_2);
	_gamescene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(e, _gamescene);
	_lplayer = static_cast<LocalPlayer*>(lp);
	_players.push_back(lp);
	auto node = lp->operator IPrimitive * ();
	_gamescene->addChild(node);
	return lp;
}


IPlayer* PlayerManager::respwanLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	_rpc->call(_selfid, "respwanNetPlayer", pos, rot);
	_lplayer->setPosition(pos);
	_lplayer->setRotation(rot);
	_lplayer->respwan();
	auto node = _lplayer->operator IPrimitive * ();
	node->setPosition(pos);
	node->setRotation(rot.x);
	auto move = MoveTo::create(1, Vec3(pos.x, pos.y, _gamescene->getDefaultCamera()->getPosition3D().z));
	_gamescene->getDefaultCamera()->runAction(move);
	return _lplayer;
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
}

void PlayerManager::update(float df)
{
	float x = 10 * (-_ka + _kd), y = 10 * (_kw - _ks);
	if (x != 0 || y != 0)
	{
		auto vsize = Director::getInstance()->getVisibleSize();
		auto cam = _gamescene->getDefaultCamera();
		Rect rc(cam->getPosition() - vsize / 2 + Size(100, 100), vsize - Size(200, 200));
		auto pos = _lplayer->_primitive->getPosition() + Vec2(x, y);
		if (!rc.containsPoint(pos))
		{
			cam->setPosition(cam->getPosition() + Vec2(x, y));
		}
		if (pos.x > GameScene::gameScene->gameRect.origin.x && pos.x < GameScene::gameScene->gameRect.size.width &&
			pos.y >GameScene::gameScene->gameRect.origin.y && pos.y < GameScene::gameScene->gameRect.size.height)
		{
			((LocalPlayer*)_lplayer)->move(x, y);
		}
	}
}

bool PlayerManager::OnColliderEntry(PhysicsContact& contact)
{
	auto a = contact.getShapeA()->getBody();
	auto b = contact.getShapeB()->getBody();
	if (a->getTag() == ::PhysicEx::NODE_TAG::ASTEROID_TAG && b->getTag() == ::PhysicEx::NODE_TAG::PLAYER_TAG)
	{
		auto player = (IPlayer*)static_cast<IPrimitive*>(b->getNode())->getOwner();
		player->onContact((Asteroid*)static_cast<IPrimitive*>(a->getNode())->getOwner());
	}
	else if (b->getTag() == ::PhysicEx::NODE_TAG::ASTEROID_TAG && a->getTag() == ::PhysicEx::NODE_TAG::PLAYER_TAG)
	{
		auto player = (IPlayer*)static_cast<IPrimitive*>(a->getNode())->getOwner();
		player->onContact((Asteroid*)static_cast<IPrimitive*>(b->getNode())->getOwner());
	}
	if (a->getTag() == ::PhysicEx::NODE_TAG::PLAYER_TAG && b->getTag() == ::PhysicEx::NODE_TAG::PLAYER_TAG)
	{
		auto player1 = (IPlayer*)static_cast<IPrimitive*>(a->getNode())->getOwner();
		auto player2 = (IPlayer*)static_cast<IPrimitive*>(b->getNode())->getOwner();
		player1->onContact(player2);
	}
	return false;
}

bool PlayerManager::OnColliderPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	auto a = contact.getShapeA()->getBody();
	auto b = contact.getShapeB()->getBody();

	return true;
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
