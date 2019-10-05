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
	std::list<T> _queue;                               // ������
	std::mutex _mutex;                                 // �������������������
	std::condition_variable _not_empty;                // ������������
	std::condition_variable _not_full;                 // ��������δ��
	int _max_size;                                     // ͬ�������������
	bool _need_stop;                                   // ֹͣ��־
};


#endif//__SYNC_QUEUE_H__
