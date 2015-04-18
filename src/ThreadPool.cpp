#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t th_num) :
max_tasks_(1024), max_threads_(th_num), task_queue_(), work_pool_(), 
manager_thread_(&ThreadPool::engine, this), engine_time_to_wait_(3), 
is_hard_stoped_(false), is_soft_stoped_(false)
{
}

ThreadPool::~ThreadPool()
{
	if (!is_hard_stoped_)
	{
		hard_stop();
	}
}

bool ThreadPool::add_task(const fn_type &fun, int priority)
{
	if (is_hard_stoped_ || is_soft_stoped_)
	{
		return false;
	}

	mutex_.lock();
	if (max_tasks_ <= task_queue_.size())
	{
		mutex_.unlock();
		return false;
	}

	fn_type_ptr fun_ptr(new fn_type(fun));
	Task task(fun_ptr, priority);
	task_queue_.push(task);
	mutex_.unlock();
	return true;
}

void ThreadPool::engine()
{
	enum task_status { NO_TASK, TASK_IS_DISTRIBUTED, TASK_IS_TAKEN };
	Task task;
	task_status status = NO_TASK;

	while (true)
	{
		//if there wasn't free worker at last iteration
		if (status != TASK_IS_TAKEN)
		{
			status = NO_TASK;
			mutex_.lock();
			if (!task_queue_.empty())
			{
				task = task_queue_.top();
				task_queue_.pop();
				status = TASK_IS_TAKEN;
			}
			mutex_.unlock();
		}

		//don't lock max_threads_
		//try to add new worker for the task
		if (status != NO_TASK && work_pool_.size() < max_threads_)
		{
			worker_ptr pWorker(new Worker(task.fn_ptr_));
			work_pool_.push_back(pWorker);
			status = TASK_IS_DISTRIBUTED;
		}

		for (std::list<worker_ptr>::iterator it = work_pool_.begin(); status == TASK_IS_TAKEN && it != work_pool_.end(); ++it)
		{
			//this status will be changed, if free worker excists
			if (!it->get()->has_task())
			{
				//don't lock max_threads_
				if (max_threads_< work_pool_.size())
				{
					work_pool_.erase(it);
				}
				else
				{
					it->get()->add_task(task.fn_ptr_);
					//put this worker to the end of list 
					work_pool_.splice(work_pool_.end(), work_pool_, it);
					status = TASK_IS_DISTRIBUTED;
				}
			}
			boost::this_thread::interruption_point();
		}
		
		boost::this_thread::interruption_point();
		//if there are no free workers, task hasn't been sent
		if (status == TASK_IS_TAKEN || status == NO_TASK)
		{
			boost::this_thread::sleep_for(engine_time_to_wait_);
		}
	}
}

void ThreadPool::hard_stop()
{
	manager_thread_.interrupt();
	is_hard_stoped_ = true;
}

size_t ThreadPool::waiting_tasks() const
{
	mutex_.lock();
	size_t t = task_queue_.size();
	mutex_.unlock();
	return t;
}

void ThreadPool::set_engine_time_to_wait(const boost::chrono::milliseconds& t)
{
	mutex_.lock();
	engine_time_to_wait_ = t;
	mutex_.unlock();
}

boost::chrono::milliseconds ThreadPool::engine_time_to_wait() const
{
	mutex_.lock();
	boost::chrono::milliseconds t = engine_time_to_wait_;
	mutex_.unlock();
	return t;
}
