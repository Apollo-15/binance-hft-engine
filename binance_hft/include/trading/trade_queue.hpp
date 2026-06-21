#pragma once

#include <boost/lockfree/spsc_queue.hpp>

template<typename T>
class TradeQueue
{
	boost::lockfree::spsc_queue<T> Queue{ 1024 };

public:
	bool Push(T const&);
	bool Pop(T& item);
};

template <typename T>
bool TradeQueue<T>::Push(T const& item)
{
	return Queue.push(item);
}

template <typename T>
bool TradeQueue<T>::Pop(T& item)
{
	return Queue.pop(item);
}
