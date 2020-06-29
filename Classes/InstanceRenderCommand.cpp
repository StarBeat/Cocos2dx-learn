#include "InstanceRenderCommand.h"
#include "IPrimitive.h"

using namespace cocos2d;

float* InstanceRenderCommand::_matbuffer = nullptr;
std::unordered_map<unsigned int, std::list<IPrimitive*>> InstanceRenderCommand::_instanceBatchPool;
std::unordered_map<unsigned int, int>InstanceRenderCommand::_hash2count;
std::unordered_map<unsigned int, PrimitiveEffect*>InstanceRenderCommand::_hash2shader;
std::unordered_map <unsigned int, std::vector<float>>InstanceRenderCommand::_hash2matbuffer;

void InstanceRenderCommand::init(float globalZOrder , Mat4 & transform, uint32_t flags, uint32_t hash)
{
	RenderCommand::init(globalZOrder, transform, flags);
	++_hash2count[hash];
}

void InstanceRenderCommand::execute()
{
	submit();
}

void InstanceRenderCommand::submit()
{
	for (auto i : _hash2count)
	{
		if (_instanceBatchPool[i.first].size() == i.second)
		{
			_matbuffer = new float[i.second * 16];
			_hash2count[i.first] = 0;
			_hash2shader[i.first]->use();
			int offset = 0;
			auto ls = _instanceBatchPool[i.first];
			for (auto ins : ls)
			{
				memcpy(_matbuffer + offset, ins->_mvp.m, 16 * sizeof(float));
				offset += 16;
			}

			_hash2shader[i.first]->bindMatBufferData(_matbuffer, ls.size());
			//_hash2shader[i.first]->bindMatBufferData(_hash2matbuffer[i.first].data(), ls.size());
			_hash2shader[i.first]->draw(ls.front()->_vertex_count, _instanceBatchPool[i.first].size());
			_hash2matbuffer[i.first].clear();
			delete[] _matbuffer;
		}
	}
}

void InstanceRenderCommand::rmPrimitive(IPrimitive* i)
{
	auto hash = i->_primitiveHash;
	_instanceBatchPool[hash].remove(i);
	if (_instanceBatchPool[hash].empty())
	{
		_instanceBatchPool.erase(hash);
	}
	updateBuffer();
}
void InstanceRenderCommand::addPrimitive(IPrimitive* i)
{
	auto hash = i->_primitiveHash;
	if (_instanceBatchPool.find(hash) == _instanceBatchPool.end())
	{
		_instanceBatchPool[hash] = std::list<IPrimitive*>();
		if (_hash2shader.find(hash) == _hash2shader.end())
		{
			_hash2shader[hash] = PrimitiveEffect::create();
			_hash2shader[hash]->genBindBuffer();
			_hash2shader[hash]->bindBufferData(i->_trianglesbuffer, i->_vertex_count);
		}
	}
	_instanceBatchPool[hash].push_back(i);
	updateBuffer();
}

void InstanceRenderCommand::pushMvp(const unsigned int& hash, cocos2d::Mat4& mvp)
{
	if (_hash2matbuffer.find(hash) == _hash2matbuffer.end())
	{
		_hash2matbuffer[hash] = std::vector<float>();
	}
	for (size_t i = 0; i < 13; i+=4)
	{
		_hash2matbuffer[hash].push_back(mvp.m[i]);
		_hash2matbuffer[hash].push_back(mvp.m[i+1]);
		_hash2matbuffer[hash].push_back(mvp.m[i+2]);
		_hash2matbuffer[hash].push_back(mvp.m[i+3]);
	}
}

void InstanceRenderCommand::updateBuffer()
{

}
