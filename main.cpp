#include <iostream>
#include "ThreadPool/thread_pool.h"
using namespace std;

void TestThreadPool();

int main()
{	
	TestThreadPool();

	return 0;
}

void TestThreadPool()
{
	ThreadPool pool;

	std::thread thd1(
		[&pool]
	{
		for (int i = 0; i < 10; i++)
		{
			auto thid = this_thread::get_id();
			pool.AddTask([thid] {cout << "同步层线程1的线程ID" << thid << endl; });
		}
	}
	);

	std::thread thd2(
		[&pool]
	{
		for (int i = 0; i < 10; i++)
		{
			auto thid = this_thread::get_id();
			pool.AddTask([thid] {cout << "同步层线程2的线程ID" << thid << endl; });
		}
	}
	);

	this_thread::sleep_for(std::chrono::seconds(2));
	pool.Stop();
	thd1.join();
	thd2.join();
}