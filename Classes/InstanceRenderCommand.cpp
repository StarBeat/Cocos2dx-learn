#include "InstanceRenderCommand.h"
using namespace cocos2d;

std::list<IPrimitive*> InstanceRenderCommand::_instances;

void InstanceRenderCommand::init(std::function<void()> func, float globalZOrder , Mat4 & transform, uint32_t flags)
{
	RenderCommand::init(globalZOrder, transform, flags);
}

void InstanceRenderCommand::execute()
{
	_func();
}

void InstanceRenderCommand::submit()
{
}
