#pragma once

template<typename T>
struct ISingle
{
private:
	ISingle(const ISingle<T>&) = delete;
	ISingle operator=(const ISingle<T>&) = delete;
protected:
	static T* instance;
	ISingle(void)
	{
		instance = static_cast<T*>(this);
	}
public:
	static T* Instane() 
	{
		if (!instance)
		{
			instance = new T();
		}
		return instance;
	}
};