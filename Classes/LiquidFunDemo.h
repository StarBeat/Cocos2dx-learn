#pragma once
#include "cocos2d.h"
#include <CCIMGUI.h>
#include <Util.h>
#include "Box2D\Box2D.h"
#include "Serializer.h"
using Util::Serializer;

struct LiquidFunDemoData
{
	struct Vec2Ex : public b2Vec2
	{
		Vec2Ex() : b2Vec2() {}
		Vec2Ex(float xx, float yy) : b2Vec2(xx, yy) {}
		Vec2Ex(const Vec2& v)
		{
			x = v.x;
			y = v.y;
		}
		Vec2Ex(const b2Vec2& v) : b2Vec2(v) {}

		operator cocos2d::Vec2()
		{
			return cocos2d::Vec2(x, y);
		}
		operator cocos2d::Vec2() const
		{
			return cocos2d::Vec2(x, y);
		}
		friend Serializer& operator >> (Serializer& in, Vec2Ex& d) {
			in >> d.x >> d.y;
			return in;
		}
		friend Serializer& operator << (Serializer& out, Vec2Ex d) {
			out << d.x << d.y;
			return out;
		}
	};
	std::string bg;
	int water_count = 0;
	int laya_count = 0;
	int chain_count = 0;
	int chain_point_count = 0;
	std::vector<Vec2Ex> waters;
	std::vector<Vec2Ex> layas;
	std::vector<int> chain_steps;
	std::vector<Vec2Ex> chains;

	Serializer sr;
	std::string path = FileUtils::getInstance()->getWritablePath() + "data.sr";
	friend Serializer& operator>> (Serializer& in, LiquidFunDemoData& d) {
		in >> d.bg >> d.water_count >> d.laya_count>> d.chain_count>>d.chain_point_count;
		Vec2Ex tmp;
		for (size_t i = 0; i < d.water_count; i++)
		{
			in >> tmp;
			d.waters.push_back(tmp);
		}
		for (size_t i = 0; i < d.laya_count; i++)
		{
			in >> tmp;
			d.layas.push_back(tmp);
		}
		for (size_t i = 0; i < d.chain_point_count; i++)
		{
			in >> tmp;
			d.chains.push_back(tmp);
		}
		int step;
		for (size_t i = 0; i < d.chain_count; i++)
		{
			in >> step;
			d.chain_steps.push_back(step);
		}
		return in;
	}
	friend Serializer& operator<< (Serializer& out, LiquidFunDemoData d) {
		out << d.bg << d.water_count << d.laya_count << d.chain_count << d.chain_point_count;
		for (size_t i = 0; i < d.water_count; i++)
		{
			out << d.waters[i];
		}
		for (size_t i = 0; i < d.laya_count; i++)
		{
			out << d.layas[i];
		}
		for (size_t i = 0; i < d.chain_point_count; i++)
		{
			out << d.chains[i];
		}
		for (size_t i = 0; i < d.chain_count; i++)
		{
			out << d.chain_steps[i];
		}
		return out;
	}

	void save()
	{
		sr << *this;
		std::string data(sr.data(), sr.size());
		FileUtils::getInstance()->writeStringToFile(data, path);
	}
	bool load()
	{
		if (!FileUtils::getInstance()->isFileExist(path))
		{
			return false;
		}
		std::string data = FileUtils::getInstance()->getStringFromFile(path);
		Util::ByteStream bs((char*)data.c_str(), data.size());
		sr(bs);
		sr >> *this;
		sr.clear();
		return true;
	}
	void clear()
	{
		water_count = 0;
		laya_count = 0;
		chain_count = 0;
		chain_point_count = 0;
		chain_steps.clear();
		waters.clear();
		layas.clear();
		chains.clear();
		bg.clear();
	}
};

class LiquidFunDemo : public cocos2d::Scene
{
public:
	virtual bool init();
	void addNewSpriteAtPosition(cocos2d::Vec2 p);
	CREATE_FUNC(LiquidFunDemo);
	cocos2d::DrawNode* _drawNode;

	bool _bslot[5];
	bool _lhold;
	std::string _sselect;
	std::vector<b2Vec2> _path;

	LiquidFunDemoData ldata;
};