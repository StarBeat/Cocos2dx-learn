#pragma once
#include "cocos2d.h"
#include <list>
struct IPrimitive;
class PrimitiveEffect;
class LightEffect;
class ShadowEffect;

class InstanceRenderCommand : public cocos2d::RenderCommand
{
public:
    InstanceRenderCommand() { _type = Type::INSTANCE_COMMAND; }
    void init(std::function<void()> func, float globalZOrder, cocos2d::Mat4& transform, uint32_t flags);
	void execute();
    void submit();
private:
    static PrimitiveEffect* _l_shader_p1;//instance render

    static std::list<IPrimitive*> _instances;
    std::function<void()> _func;//准备数据

    friend class IPrimitive;
};
