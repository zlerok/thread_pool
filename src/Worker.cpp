#include "ThreadPool.h"

ThreadPool::Worker::Worker(const fn_type_ptr &task) : task_(task), has_task_(true), th_(&Worker::make_task, this), is_enable_(true)
{
}

ThreadPool::Worker::~Worker()
{
	is_enable_ = false;
}

void ThreadPool::Worker::make_task()
{
	boost::chrono::milliseconds sleep_time(3);

	while (is_enable_)
	{
		if (has_task_)
		{
			task_.get()->operator()();	
			has_task_= false;
		}
		boost::this_thread::sleep_for(sleep_time);
	}
}

bool ThreadPool::Worker::add_task(const fn_type_ptr &new_task)
{
	if (!has_task_)
	{
		task_ = new_task;
		has_task_ = true;
		return true;
	}
	return false;
}

bool ThreadPool::Worker::has_task() const
{
	return has_task_;
}