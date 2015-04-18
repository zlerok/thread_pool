#pragma once
#include <queue>
#include <list>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>

class ThreadPool
{
public:
	typedef boost::function<void()> fn_type;
	typedef boost::shared_ptr<fn_type> fn_type_ptr;

	ThreadPool(size_t th_num);

    //adds function to the queue
	//task - function without arguments and any return value;
	//priority - priority of the task
	//returns true, if task added to queue
	//todo: erase exception
	bool add_task(const boost::function<void()> &task, int priority); 
	
	//stops to task recieve by thread pool
	void soft_stop() { is_soft_stoped_ = true; }
	
	//stops to task send to execution
	void hard_stop();

	bool is_hard_stoped() const { return is_hard_stoped_; }
	bool is_soft_stoped() const { return is_soft_stoped_; }
	
	//returns max thread number	
	size_t max_threads() const { return max_threads_; }
	
	//sets max thread number
	//1) if current threads number is bigger, free threads will be deleted
	//if there are no free threads, threads number will be decreased, then threads will be freed
	//2) if current threads number is smaller, threads will be increased, then it will be necessary for new tasks 
	void set_max_threads(size_t n) { max_threads_ = n; }
	
    //returns task number in queue
	size_t waiting_tasks() const;
	
	//returns max task number in queue
	size_t max_tasks() const { return max_tasks_; }

	//sets timer for engine
	//if engine hasn't worker to give him task, it will be wait for
	//engine_time_to_wait_ milliseconds
	void set_engine_time_to_wait(const boost::chrono::milliseconds& t);
	boost::chrono::milliseconds engine_time_to_wait() const ;

    //set max task number
	void set_max_tasks(size_t ceil) { max_tasks_ = ceil; }

	~ThreadPool();

private:	
  	class Worker
	{
	public:
		//constructor gets pointer to task for execution
		Worker(const fn_type_ptr &new_task);
		
		//gets pointer to task for execution
		//returns false, if worker has another task
		//returns true, if  new_task is accepted
		bool add_task(const fn_type_ptr &new_task);
		
		//returns true, if worker is busy
		//false, if worker if free
		bool has_task() const;
		~Worker();
	private:
		Worker(const Worker &);
		boost::mutex mutex_;
		bool has_task_;
		bool is_enable_;
		boost::thread th_;
		fn_type_ptr task_;

		void make_task();
	};

    //discript task with priority
    //is neccesary to puts in priority_queue
	struct Task
	{
		fn_type_ptr fn_ptr_;
		size_t priority_;

		bool operator<(const Task & t1) const { return (priority_ < t1.priority_); }

		Task() : fn_ptr_(), priority_(0) {}
		Task(fn_type_ptr f, size_t p) : fn_ptr_(f), priority_(p) {}
		
	};

	typedef std::shared_ptr<Worker> worker_ptr;

	ThreadPool(const ThreadPool &);
	ThreadPool& operator=(const ThreadPool&);

    //engine takes task from queue, choose worker and sends the task to this worker
    //reduce warks count, if it's necessary
    //adds new worker if max_threads_ allows
	void engine();

	//timer for engine
	//if engine hasn't worker to give him task, it will be wait for
	//engine_time_to_wait_ milliseconds
	//default value is 3
	boost::chrono::milliseconds engine_time_to_wait_;

	//max threads number
	size_t max_threads_;

	//max task number in queue
	//default value = 1024
	size_t max_tasks_;

	bool is_hard_stoped_;
	bool is_soft_stoped_;
	
	std::priority_queue<Task> task_queue_;
	std::list<worker_ptr> work_pool_;

	mutable boost::mutex mutex_;

	//function engine() executs in manager_thread_
	boost::thread manager_thread_;		
};



