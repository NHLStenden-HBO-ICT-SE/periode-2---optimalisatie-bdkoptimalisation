#pragma once

namespace Tmpl8
{

class ThreadPool; //Forward declare

class Worker;

class Worker
{

  public:
    //Instantiate the worker class by passing and storing the threadpool as a reference
    Worker(ThreadPool& s) : pool(s) {}

    inline void operator()();
  private:
    ThreadPool& pool;
};

class ThreadPool
{
  private:
    friend class Worker; //Gives access to the private variables of this class
    const uint max_Threads = std::thread::hardware_concurrency();
    atomic<int> available_threads{ std::thread::hardware_concurrency() - 1};
    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;

    std::condition_variable condition; //Wakes up a thread when work is available

    std::mutex queue_mutex; //Lock for our queue
    bool stop = false;

  public:
    bool  threads_available() {
        return (available_threads > 0);
    }

    uint get_thread_count() {
        return max_Threads;
    }

    ThreadPool(size_t numThreads) : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
            workers.push_back(std::thread(Worker(*this)));
    }

    ThreadPool() : stop(false)
    {
        for (size_t i = 0; i < (std::thread::hardware_concurrency() - 1); ++i)
            workers.push_back(std::thread(Worker(*this)));
    }

    ~ThreadPool()
    {
        stop = true; // stop all threads
        condition.notify_all();

        for (auto& thread : workers)
            thread.join();
    }

    template <class T>
    auto enqueue(T task) -> std::future<decltype(task())>
    {
        available_threads--;
        //Wrap the function in a packaged_task so we can return a future object
        auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));

        //Scope to restrict critical section
        {
            //lock our queue and add the given task to it
            std::unique_lock<std::mutex> lock(queue_mutex);

            tasks.push_back([=] {
                (*wrapper)();
            });
        }

        //Wake up a thread to start this task
        condition.notify_one();


        return wrapper->get_future();
    }

};

inline void Worker::operator()()
{
    std::function<void()> task;
    while (true)
    {
        //Scope to restrict critical section
        //This is important because we don't want to hold the lock while executing the task,
        //because that would make it so only one task can be run simultaneously (aka sequantial)
        {
            std::unique_lock<std::mutex> locker(pool.queue_mutex);

            //Wait until some work is ready or we are stopping the threadpool
            //Because of spurious wakeups we need to check if there is actually a task available or we are stopping
            pool.condition.wait(locker, [=] { return pool.stop || !pool.tasks.empty(); });

            if (pool.stop) break;

            task = pool.tasks.front();
            pool.tasks.pop_front();
        }

        task();
    }
    pool.available_threads++;
}

} // namespace Tmpl8