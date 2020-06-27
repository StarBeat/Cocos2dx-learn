#pragma once
#include "cocos2d.h"

class Effect : public cocos2d::Ref
{
public:
	auto inline getGLProgramState() const noexcept { return _glprogramstate; }
	void useGLProgramState(cocos2d::Node* rhis) 
	{
		CCASSERT(_glprogramstate!=nullptr, __FUNCDNAME__);
		rhis->setGLProgramState(_glprogramstate); 
	}
	Effect();
	virtual ~Effect();
protected:
	bool initGLProgramState(const std::string& vs = "", const std::string& fs = "");

protected:
	cocos2d::GLProgramState* _glprogramstate;
};

