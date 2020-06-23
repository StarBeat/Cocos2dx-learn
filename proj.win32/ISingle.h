#pragma once

template<typename T>
struct ISingle
{
protected:
	static T* instance;
public:
	static T* Instane() 
	{
		return instance;
	}
};