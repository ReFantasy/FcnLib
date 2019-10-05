#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>
#include "sync_queue.h"

class ThreadPool
{
	
public:
	using Task = std::function<void()>;

	ThreadPool(int max_task_count = 1024, int num_threads = std::thread::hardware_concurrency());
	~ThreadPool();

	void Stop();

	void AddTask(Task &&task);
	
	void AddTask(const Task &task);
	
private:
	void Start(int num_threads);

	void RunInThread();

	void StopThreadGroup();
	
private:
	std::list<std::shared_ptr<std::thread>> _thread_group;  // 处理任务的线程
	SyncQueue<Task> _queue; // 同步队列
	std::atomic<bool> _running;// 是否停止
	std::once_flag _flag;
};


#endif//__THREAD_POOL_H__
