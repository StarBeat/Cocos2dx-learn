#pragma once
#include "cocos2d.h"

class Effect : public cocos2d::Ref
{
public:
	auto getGLProgramState() const noexcept { return _glprogramstate; }
	void useGLProgramState(cocos2d::Node* rhis) { CCASSERT(_glprogramstate!=nullptr, __FUNCDNAME__); rhis->setGLProgramState(_glprogramstate); }
protected:
	bool initGLProgramState(const std::string& fs, const std::string& vs = "");
	Effect();
	virtual ~Effect();

protected:
	cocos2d::GLProgramState* _glprogramstate;
};

