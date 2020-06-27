#include "Light2d.h"
#include "PhysicExt.h"
#include <math\CCMath.h>
#include <math.h>
#include "MathUtil.h"

using namespace cocos2d;
using cocos2d::MathUtil;

template<>
LightingManager* ISingle<LightingManager>::instance = new LightingManager();

static V2F_C4B_T2F calculateTriangle()
{

}

Light2d* Light2d::create()
{
	auto ins = new Light2d();
	ins->retain();
	return ins;
}

Light2d::Light2d()
{
	_shadoweff = ShadowEffect::create(this);
	_lighteff = LightEffect::create(this);

	_shadoweff->genBuffer();
	_lighteff->genBuffer();
	/*auto dn = DrawNode::create();
	this->addChild(dn);
	dn->drawCircle(getPosition(), _lighteff->_lightDistance, 0, 50, false, Color4F::GREEN);*/
	LightingManager::Instane()->lights.insert(std::make_pair(this, Mat4::ZERO));
}

void Light2d::debugDrawline(Vec2 a, Vec2 b,const Color4F& cl)
{
	static DrawNode* node = nullptr;
	if (node == nullptr)
	{
		node =DrawNode::create();
		this->addChild(node);
	}
	node->drawLine(a, b, cl);
}
void Light2d::calculateShadow()
{
	_shadowtrianglesbuffer.clear();
	//calculate
	std::vector<std::pair<int, Vec2*>> hits;
	int hit = ::PhysicEx::OverlapCircle(getPosition(), _lighteff->_lightDistance, hits);
	if (hit > 0)
	{
		Vec3 z(0, 0, 1);
		float R_2 = _lighteff->_lightDistance * _lighteff->_lightDistance;
		float r_2 = _lighteff->_lightVolumn * _lighteff->_lightVolumn;
		for (auto i : hits)//遍历检测到的physic shap
		{
			auto v = i.second;
			for (size_t j = 0; j < i.first; j++)//遍历计算的边界点
			{
				auto _p0 = this->convertToNodeSpace(v[(j + 1) % i.first]);
				Vec2 _p1 = this->convertToNodeSpace(v[j]);
				Vec3 p0(_p0.x, _p0.y, 0);
				Vec3 p1(_p1.x, _p1.y, 0);

				auto ang0 = asinf(_lighteff->_lightVolumn / p0.length());
				auto ang1 = asinf(_lighteff->_lightVolumn / p1.length());

				
				auto shadowa = ::MathUtilEx::Normalize(::MathUtilEx::Rotate(p0, -ang0)) * (sqrtf(R_2 - r_2) - p0.length() * cosf(ang0));
				auto shadowb = ::MathUtilEx::Normalize(::MathUtilEx::Rotate(p1, ang1)) * (sqrtf(R_2 - r_2) - p1.length() * cosf(ang1));
				shadowa += p0;
				shadowb += p1;
				int type = 0;
				if (MathUtilEx::Cross((p1 - p0), (shadowb - p1)).z >=0)
				{
					type |= 1;
					shadowb = ::MathUtilEx::Normalize(MathUtilEx::Rotate(p0, ang0)) * (sqrt(R_2 - r_2) - p0.length() * cosf(ang0));
					shadowb += p0;
				}
				if ((MathUtilEx::Cross(p0 - shadowa, p1 - p0).z >= 0))
				{
					type |= 2;
					shadowa = ::MathUtilEx::Normalize(MathUtilEx::Rotate(p1, -ang1) )* (sqrtf(R_2 - r_2) - p1.length() * cosf(ang1));
					shadowa += p1;
				}

				auto OC = (shadowa + shadowb) / 2;
				
				Vec3 shadowr = ::MathUtilEx::Normalize(OC)* (R_2 / OC.length());
				switch (type)
				{
				case 0:
				{
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p0), _shadow_color, MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowa), _shadow_color,  MathUtilEx::ToVec2(p0) ,
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });

					
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p1), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p0), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });

					
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p1), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowb), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					break;
				}
				case 1:
				{
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p0), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowa), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });

					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p0), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowb), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					break;
				}
				case 2:
				{
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p1), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowa), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });

					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p1), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowb), _shadow_color,  MathUtilEx::ToVec2(p0),
						MathUtilEx::ToVec2(p1) });
					break;
				}
				case 3:
				{
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p1), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowa), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });

					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p0), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p1), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });

					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(p0), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowr), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					_shadowtrianglesbuffer.push_back(SS_Vertice{
						MathUtilEx::ToVec2(shadowb), _shadow_color,  MathUtilEx::ToVec2(p1),
						MathUtilEx::ToVec2(p0) });
					break;
				}
				default:
					break;
				}
			/*	debugDrawline(MathUtilEx::ToVec2(p0), MathUtilEx::ToVec2(p1), Color4F::RED);
				debugDrawline(MathUtilEx::ToVec2(p1), MathUtilEx::ToVec2(shadowb), Color4F::GREEN);
				debugDrawline(MathUtilEx::ToVec2(p0), MathUtilEx::ToVec2(shadowa), Color4F::BLUE);
				debugDrawline(MathUtilEx::ToVec2(shadowa), MathUtilEx::ToVec2(shadowr), Color4F::WHITE);
				debugDrawline(MathUtilEx::ToVec2(shadowb), MathUtilEx::ToVec2(shadowr), Color4F::YELLOW);*/
			}
			delete i.second;
		}
	}
	int size = _shadowtrianglesbuffer.size();
	SS_Vertice* buffer = _shadowtrianglesbuffer.data();
	if (size > 0)
	{
		_shadoweff->bindBufferData(buffer, size);
	}
}

void Light2d::renderLighting(const cocos2d::Mat4& transform)
{
	_lighteff->use(transform);
	_lighteff->draw(_shadoweff->getShadowMap());
}

void Light2d::renderShadow(const cocos2d::Mat4& transform)
{
	_shadoweff->use(transform);
	calculateShadow();
	_shadoweff->draw(_shadowtrianglesbuffer.size());
}

void Light2d::drawShadowAndLight(const cocos2d::Mat4& transform)
{
	renderShadow(transform);
	renderLighting(transform);
}

void Light2d::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	LightingManager::Instane()->pushMat(this, transform);
}
