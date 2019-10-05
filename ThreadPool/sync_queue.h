#ifndef __SYNC_QUEUE_H__
#define __SYNC_QUEUE_H__
#include <list>
#include <mutex>
#include <condition_variable>
#include <iostream>

template<typename T>
class SyncQueue
{
public:
	SyncQueue(int max_size) :_max_size(max_size), _need_stop(false) {}

	void Put(const T &x)
	{
		Add(x);
	}
	void Put(T &&x)
	{
		Add(std::forward<T>(x));
	}

	void Take(std::list<T> &list)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_not_empty.wait(locker, [this] { return _need_stop || NotEmpty(); });
		if (_need_stop)
			return;
		list = std::move(_queue);
		_not_full.notify_one();
	}
	void Take(T &t)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_not_empty.wait(locker, [this] { return _need_stop || NotEmpty(); });
		if (_need_stop)
			return;
		t = _queue.front();
		_queue.pop_front();
		_not_full.notify_one();
	}

	void Stop()
	{
		{
			std::lock_guard<std::mutex> locker(_mutex);
			_need_stop = true;
		}
		_not_full.notify_all();
		_not_empty.notify_all();
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> locker(_mutex);
		return _queue.empty();
	}
	bool Full()
	{
		std::lock_guard<std::mutex> locker(_mutex);
		return _queue.size() == _max_size;
	}
	size_t Size()
	{
		std::lock_guard<std::mutex> locker(_mutex);
		return _queue.size();
	}
	int Count()
	{
		return _queue.size();
	}


private:
	bool NotFull()const
	{
		bool full = _queue.size() >= _max_size;
		return !full;
	}
	bool NotEmpty()const
	{
		bool empty = _queue.empty();
		return !empty;
	}

	template<typename F>
	void Add(F&& x)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_not_full.wait(locker, [this] {return _need_stop || NotFull(); });
		if (_need_stop)
			return;
		_queue.push_back(std::forward<F>(x));
		_not_empty.notify_one();
	}

private:
	std::list<T> _queue;                               // 缓冲区
	std::mutex _mutex;                                 // 互斥量和条件变量结合
	std::condition_variable _not_empty;                // 条件变量不空
	std::condition_variable _not_full;                 // 条件变量未满
	int _max_size;                                     // 同步队列最大容量
	bool _need_stop;                                   // 停止标志
};


#endif//__SYNC_QUEUE_H__
