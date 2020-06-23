#pragma once
#include "Serializer.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <list>
namespace Util
{
class SimpleNet;
struct Message;

template<typename T>
struct type_xx { typedef T type; };

template<>
struct type_xx<void> { typedef int8_t type; };


// �������ģ��
template<typename Tuple, std::size_t... Is>
void package_params_impl(Serializer& ds, const Tuple& t, std::index_sequence<Is...>)
{
	initializer_list<int>{((ds << std::get<Is>(t)), 0)...};
}

template<typename... Args>
void package_params(Serializer& ds, const std::tuple<Args...>& t)
{
	package_params_impl(ds, t, std::index_sequence_for<Args...>{});
}

// ��tuple���������ú���ģ����
template<typename Function, typename Tuple, std::size_t... Index>
void invoke_impl(Function&& func, Tuple&& t, std::index_sequence<Index...>)
{
	func(std::get<Index>(std::forward<Tuple>(t))...);
}

template<typename Function, typename Tuple>
void invoke(Function&& func, Tuple&& t)
{
	constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
	invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
}

// ���ð����࣬��Ҫ���ڷ����Ƿ�void�����
template<typename R, typename F, typename ArgsTuple>
typename std::enable_if<std::is_same<R, void>::value, typename type_xx<R>::type >::type
call_helper(F f, ArgsTuple args) {
	invoke(f, args);
	return 0;
}

template<typename R, typename F, typename ArgsTuple>
typename std::enable_if<!std::is_same<R, void>::value, typename type_xx<R>::type >::type
call_helper(F f, ArgsTuple args) {
	//��ֹ����ֵ
	static_assert(false);
}

class Rpc
{
public:
	enum class CSType
	{
		RPC_Client,
		RPC_Server
	};
	const static int SERVERID = -2;
	const static int DEFAULT_BINDID = -1;
	Rpc(std::function<void(int)> bindEnty);
	~Rpc();

	void asClient(std::string& ip, int port);
	void asServer(int port);

	void shutdown();

	void send(Message* msg);
	void recv(Message** msg);
	void serverRun();
	void update();
public:
	void unbind(int id);
	template<typename F>
	void bind(int id, std::string name, F func)
	{
		if (_handlers.find(id) == _handlers.end())
		{
			_handlers[id] = std::unordered_map < std::string, proxyfunc>();
		}
		_handlers[id][name] = std::bind(&Rpc::callproxy<F>, this, func, std::placeholders::_1, std::placeholders::_2);
	}

	template<typename F, typename S>
	void bind(int id, std::string name, F func, S* s)
	{
		if (_handlers.find(id) == _handlers.end())
		{
			_handlers[id] = std::unordered_map < std::string, proxyfunc>();
		}
		_handlers[id][name] = std::bind(&Rpc::callproxy<F, S>, this, func, s, std::placeholders::_1, std::placeholders::_2);
	}

	template<typename... Params>
	void call(std::string name, Params... ps) {
		using args_type = std::tuple<typename std::decay<Params>::type...>;
		args_type args = std::make_tuple(ps...);

		Serializer ds;
		if (_type == CSType::RPC_Server)
		{
			for (auto id : _ids)
			{
				ds << name;
				ds << SERVERID;
				package_params(ds, args);
				net_call(ds, id);
				ds.clear();
			}
			return;
		}
		ds << name;
		ds << 0;
		package_params(ds, args);
		net_call(ds, 0);
	}

	void call(std::string name) {
		Serializer ds;
		if (_type == CSType::RPC_Server)
		{
			for (auto id : _ids)
			{
				ds << name;
				ds << SERVERID;
				net_call(ds, id);
				ds.clear();
			}
			return;
		}
		ds << name;
		ds << 0;
		net_call(ds, 0);
	}

private:
	/// single_call for server
	template<typename... Params>
	void single_call(int id, std::string name, Params... ps) {
		using args_type = std::tuple<typename std::decay<Params>::type...>;
		args_type args = std::make_tuple(ps...);
		Serializer ds;
		ds << name;
		ds << SERVERID;//source
		package_params(ds, args);
		net_call(ds, id);
	}
	void single_call(int id, std::string name)
	{
		Serializer ds;
		ds << name;
		ds << SERVERID;
		net_call(ds, id);
	}
	
	void connect(int id);
	void disconnect(int id);
	
	void call_(int id, std::string name, const char* data, int len);

	void net_call(Serializer& ds, int id);
	


	template<typename F>
	inline void callproxy(F fun, const char* data, int len)
	{
		callproxy_(fun, data, len);
	}

	template<typename F, typename S>
	inline void callproxy(F fun, S* s,const char* data, int len)
	{
		callproxy_(fun, s, data, len);
	}
	template<typename R, typename... Params>
	void callproxy_(R(*func)(Params...), const char* data, int len) 
	{
		callproxy_(std::function<R(Params...)>(func), data, len);
	}

	// ���Ա����ָ��
	template<typename R, typename C, typename S, typename... Params>
	void callproxy_(R(C::* func)(Params...), S* s, const char* data, int len) 
	{

		using args_type = std::tuple<typename std::decay<Params>::type...>;
		
		Serializer ds(ByteStream(data, len));
		constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
		args_type args = ds.getTuple < args_type >(std::make_index_sequence<N>{});

		auto ff = [=](Params... ps)->R {
			return (s->*func)(ps...);
		};
		call_helper<R>(ff, args);
	}

	// functional
	template<typename R, typename... Params>
	void callproxy_(std::function<R(Params... ps)> func, const char* data, int len) 
	{

		using args_type = std::tuple<typename std::decay<Params>::type...>;

		Serializer ds(ByteStream(data, len));
		constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
		args_type args = ds.getTuple < args_type >(std::make_index_sequence<N>{});

		call_helper<R>(func, args);
	}
	using proxyfunc = std::function<void(const char*, int)>;
	std::unordered_map<int, std::unordered_map<std::string, proxyfunc>> _handlers;
	std::list<int> _ids;
	std::function<void(int)> _bindEnty;
	CSType _type;
#pragma warning (disable: 4430) 
	SimpleNet* _net;
};
}
