#include <iostream>  
#include <fstream>
#include <vector>
#include "ThreadPool.h"

std::ofstream out("log.txt");

int global_task_number = 0;

void count_to_ten()
{
	global_task_number++;
	int task_number = global_task_number;
	std::cout << task_number << "start task\n";
	out << task_number << "start task\n"; 
	
	for (int i = 0; i < 10; i++)
	{
		std::cout	<< i << "\t task: "<< task_number <<"\t thread  " << boost::this_thread::get_id() << std::endl;
		out			<< i << '\t' << "thread  " << boost::this_thread::get_id() << std::endl;
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
	}
	std::cout << task_number << "stop task\n";
	out << task_number << "stop task\n";
}

void my_print(int i, double f)
{
	std::cout << i << ' ' << f << std::endl;
	boost::this_thread::sleep_for(boost::chrono::seconds(1));
}

int main(int argc, char* argv[])
{
	ThreadPool pool(2);
	pool.set_max_tasks(5);
	boost::function<void()> f(count_to_ten);
	boost::function<void()> g(boost::bind(my_print, 3, 9.0));

	pool.add_task(f, 1);
	pool.add_task(f, 3);
	pool.add_task(f, 2);
	boost::this_thread::sleep_for(boost::chrono::seconds(6));
	pool.set_max_threads(3);

	pool.add_task(f, 6);
	pool.add_task(f, 1);
	std::cout << "soft_stop\n";
	out << "soft_stop\n";
	pool.soft_stop();


	std::cout << "added 6th task\n";
	out << "added 6th task\n";
	pool.add_task(f, 3);
	pool.add_task(f, 1);
	pool.add_task(f, 0);


	boost::this_thread::sleep_for(boost::chrono::seconds(6));
	std::cout << "hard_stop\n";
	out << "hard_stop\n";
	pool.hard_stop();


	boost::this_thread::sleep_for(boost::chrono::seconds(30));
	return 0;
}