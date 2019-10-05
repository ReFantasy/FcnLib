#include "thread_pool.h"


ThreadPool::ThreadPool(int max_task_count , int num_threads) :_queue(max_task_count)
{
	Start(num_threads);
}

ThreadPool::~ThreadPool()
{
	Stop();
}

void ThreadPool::Stop()
{
	std::call_once(_flag, [this] {StopThreadGroup(); });
}

void ThreadPool::AddTask(Task &&task)
{
	_queue.Put(std::forward<Task>(task));
}

void ThreadPool::AddTask(const Task &task)
{
	_queue.Put(task);
}

void ThreadPool::Start(int num_threads)
{
	_running = true;
	// 创建线程组
	for (int i = 0; i < num_threads; i++)
	{
		_thread_group.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
	}
}

void ThreadPool::RunInThread()
{
	while (_running)
	{
		// 取任务分别执行
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

void ThreadPool::StopThreadGroup()
{
	_queue.Stop();  // 同步队列的线程停止
	_running = false;
	for (auto thread : _thread_group)
	{
		if (thread)
			thread->join();
	}
	_thread_group.clear();
}

