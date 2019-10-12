#include <iostream>
#include "ThreadPool/thread_pool.h"
#include "SerialPort/serial_port.h"
using namespace std;

#ifdef _WIN32
void TestThreadPool();
#endif

int main()
{	
	
#ifdef _WIN32
	cout << "hello" << endl;
#endif
	return 0;
}

#ifdef _WIN32
void TestThreadPool()
{
	ThreadPool pool;

	std::thread thd1(
		[&pool]
	{
		for (int i = 0; i < 100; i++)
		{
			auto thid = this_thread::get_id();
			pool.AddTask([thid] {cout << "layer1 thread ID" << thid << endl; });
		}
	}
	);

	std::thread thd2(
		[&pool]
	{
		for (int i = 0; i < 100; i++)
		{
			auto thid = this_thread::get_id();
			pool.AddTask([thid] {cout << "layer2 thread ID" << thid << endl; });
		}
	}
	);

	this_thread::sleep_for(std::chrono::seconds(2));
	pool.Stop();
	thd1.join();
	thd2.join();
}
#endif