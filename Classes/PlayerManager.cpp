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

		_selfid = id;
		_rpc->bind(id,"setSeed",&GameManager::setSeed, GameManager::Instane());
		_connected = true;
		auto gs = GameScene::gameScene;
		auto player = PlayerManager::Instane()->createLocalPlayer(Vec2(0, 0), Vec2(0, 0));
		gs->addChild(player->operator IPrimitive * ());
		auto e = EventListenerKeyboard::create();
		e->onKeyPressed = [player](EventKeyboard::KeyCode k, Event* e)
		{
			float x = 0, y = 0;
			auto b = e->isStopped();
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
			default:
				break;
			}
			((LocalPlayer*)player)->move(x, y);
		};
		gs->getEventDispatcher()->addEventListenerWithSceneGraphPriority(e, gs);
	}
#pragma region Binds

#pragma endregion


}


void PlayerManager::createNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	CCLOG("createNetPlayer");
}

IPlayer* PlayerManager::createLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	_rpc->call(Util::Rpc::DEFAULT_BINDID, "createNetPlayer", pos, rot);
	IPlayer* lp;
	if (!_lpool.empty())
	{
		lp = _lpool.top();
		_lpool.pop();
		lp->setPosition(pos);
		lp->setRotation(rot);
	//	lp->operator IPrimitive* ()->isVisible(); false don`t addchild
		return lp;
	}
	lp = LocalPlayer::create(pos, rot);
	return lp;
}

IPlayer* PlayerManager::respwanNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	return nullptr;
}

IPlayer* PlayerManager::respwanLocalPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	return nullptr;
}
