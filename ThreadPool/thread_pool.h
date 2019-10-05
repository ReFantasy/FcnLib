#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>
#include "sync_queue.h"

const int MaxTaskCount = 100;

class ThreadPool
{
public:
	using Task = std::function<void()>;
	ThreadPool(int num_threads = std::thread::hardware_concurrency()) :_queue(MaxTaskCount)
	{
		Start(num_threads);
	}
	~ThreadPool()
	{
		Stop();
	}

	void Stop()
	{
		std::call_once(_flag, [this] {StopThreadGroup(); });
	}

	void AddTask(Task &&task)
	{
		_queue.Put(std::forward<Task>(task));
	}
	void AddTask(const Task &task)
	{
		_queue.Put(task);
	}


private:
	void Start(int num_threads)
	{
		_running = true;
		// �����߳���
		for (int i = 0; i < num_threads; i++)
		{
			_thread_group.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
		}
	}

	void RunInThread()
	{
		while (_running)
		{
			// ȡ����ֱ�ִ��
			std::list<Task> list;
			_queue.Take(list);
			for (auto&task : list)
			{
				if (!_running)
					return;
				task();
			}
		}
	}

	void StopThreadGroup()
	{
		_queue.Stop();  // ͬ�����е��߳�ֹͣ
		_running = false;
		for (auto thread : _thread_group)
		{
			if (thread)
				thread->join();
		}
		_thread_group.clear();
	}
private:
	std::list<std::shared_ptr<std::thread>> _thread_group;  // ����������߳�
	SyncQueue<Task> _queue; // ͬ������
	std::atomic<bool> _running;// �Ƿ�ֹͣ
	std::once_flag _flag;
};


#endif//__THREAD_POOL_H__
