#pragma once
#include "cocos2d.h"
#include "PrimitiveEffect.h"
#include <list>
struct IPrimitive;
class PrimitiveEffect;
class LightEffect;
class ShadowEffect;

class InstanceRenderCommand : public cocos2d::RenderCommand
{
public:
    InstanceRenderCommand() 
    {
        _type = Type::INSTANCE_COMMAND; 
    }
    void init(float globalZOrder, cocos2d::Mat4& transform, uint32_t flags, uint32_t hash);
    static void init()
    {
    }
	void execute();
    void submit();
    void rmPrimitive(IPrimitive* i);
    void addPrimitive(IPrimitive* i);
    void pushMvp(const unsigned int& hash, cocos2d::Mat4& mvp);
private:
    void updateBuffer();

private:
    static std::unordered_map<unsigned int, std::list<IPrimitive*>> _instanceBatchPool;
    static std::unordered_map<unsigned int, int>_hash2count;
    static std::unordered_map<unsigned int, PrimitiveEffect*>_hash2shader;
    static std::unordered_map <unsigned int, std::vector<float>>_hash2matbuffer;


    friend class IPrimitive;
};
