#pragma once

#include <vector>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include <mutex>

namespace Base_ns {

class ThreadPool {
private:
    std::mutex poolMutex;
    class Worker {
    public:
        std::mutex ownershipMutex;
        // std::unique_lock<std::mutex> ownershipLock;
        // volatile std::atomic<bool> isRunning;
        volatile std::atomic<bool> start;
        volatile std::atomic<bool> exit;
        std::shared_ptr<std::thread> thread;
        std::mutex runMutex;
        std::condition_variable runCond;
        std::function<void(void)> task;
        Worker() : start(false), exit(false){
            thread = std::make_shared<std::thread>([this]() {
                // Note: for std::atomic, the default memory model is std::memory_order_seq_cst
                while (true) {
                    std::unique_lock lock(runMutex);
                    runCond.wait(lock, [this]() {
                        return start.load() || exit.load();
                    });
                    if (exit.load()) { break; }
                    std::atomic_thread_fence(std::memory_order_release); // runs task only after setting flag
                    task();
                    std::atomic_thread_fence(std::memory_order_acquire); // sets flag only after finishing task
                    start.store(false, std::memory_order_seq_cst);
                    std::atomic_thread_fence(std::memory_order_release);
                    // FYI: to make sure that setting isRunning to false strictly after setting start to false
                    ownershipMutex.unlock();
                }
            });
        }
        bool isTaskRunning() const { return start; }
        void runTask(std::function<void(void)> tsk) {
            this->task = tsk;
            assert(start == false);
            start.store(true, std::memory_order_seq_cst);
            std::atomic_thread_fence(std::memory_order_release);
            runCond.notify_one();
        }
        bool tryAcquire() {
            return ownershipMutex.try_lock();
        }
        ~Worker() {
            exit.store(true, std::memory_order_seq_cst);
            runCond.notify_all();
            thread->join();
        }
    };
    std::vector<std::shared_ptr<Worker> > pool;

public:
    explicit ThreadPool(size_t pool_size) {
        for (size_t i = 0; i < pool_size; i++) {
            pool.push_back(std::make_shared<Worker>());
        }
    }

    void asyncRunTask(std::function<void(void)> task) {
        std::lock_guard<std::mutex> lock(this->poolMutex);
        // --- Find unused thread from pool
        size_t worker_idx = 0;
        size_t pool_size = pool.size();
        while (true) {
            if ((pool[worker_idx % pool_size]->tryAcquire())) { break; }
            if ((worker_idx != 0) && (worker_idx % pool.size() == 0)) {
                LOG_DEBUG("No enougn threads, spin finding...");
            }
            worker_idx ++;
        }
        std::atomic_thread_fence(std::memory_order_seq_cst);
        // --- Run task in thread
        pool[worker_idx % pool_size]->runTask(task);
    }

    ~ThreadPool() {
        // Worker automatically release resource inside destruct function, no need to stop manually
    }

};

}
