#pragma once
#include <queue>
#include <atomic>
#include <initializer_list>

using std::queue;
using std::atomic;
using std::initializer_list;

namespace Util
{
template<typename T>
class Queue : protected queue<T>
{
public:

	Queue() : queue<T>::queue()
	{
	}

	~Queue()
	{
		queue<T>::~queue();
	}

	void push(const T& v)
	{
		lock_guard l(_busy);
		queue<T>::push(v);
	}

	void push(T&& v)
	{
		lock_guard l(_busy);
		queue<T>::push(std::forward<T>(v));
	}

	template <class... Args>
	void emplace(Args&&... args)
	{
		lock_guard l(_busy);
		queue<T>::emplace(std::forward<Args>(args)...);
	}

	size_t size()
	{
		lock_guard l(_busy);
		return queue<T>::size();
	}

	bool empty()
	{
		lock_guard l(_busy);
		return queue<T>::empty();
	}

	void swap(queue<T>& _Right)
	{
		lock_guard l(_busy);
		queue<T>::swap(_Right);
	}
	
	T& pop() 
	{
		lock_guard l(_busy);
		auto rt = queue<T>::front();
		queue<T>::pop();
		return rt;
	}

	T& front()
	{
		lock_guard l(_busy);
		return queue<T>::front();
	}
	T& back()
	{
		lock_guard l(_busy);
		return queue<T>::back();
	}
private:
	struct lock_guard
	{
		lock_guard(atomic<bool>& a) : __busy(a)
		{
			bool expect = false;
			while (!__busy.compare_exchange_weak(expect, true))
			{
				expect = false;
			}
		}
		~lock_guard()
		{
			__busy.store(false);
		}
		atomic<bool>& __busy;
	};
	atomic<bool> _busy = false;
};
}