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
        volatile std::atomic<bool> isRunning;
        volatile std::atomic<bool> start;
        volatile std::atomic<bool> exit;
        std::shared_ptr<std::thread> thread;
        std::mutex runMutex;
        std::condition_variable runCond;
        std::function<void(void)> task;
        Worker() : isRunning(false), start(false), exit(false) {
            thread = std::make_shared<std::thread>([this]() {
                while (true) {
                    std::unique_lock lock(runMutex);
                    runCond.wait(lock, [this]() {
                        return start.load(std::memory_order_seq_cst) || exit.load(std::memory_order_seq_cst);
                    });
                    if (exit.load(std::memory_order_seq_cst)) { break; }
                    isRunning.store(true, std::memory_order_seq_cst);
                    std::atomic_thread_fence(std::memory_order_release); // runs task only after setting flag
                    task();
                    std::atomic_thread_fence(std::memory_order_acquire); // sets flag only after finishing task
                    start.store(false, std::memory_order_seq_cst);
                    isRunning.store(false, std::memory_order_seq_cst);
                    // FYI: to make sure that setting isRunning to false strictly after setting start to false
                }
            });
        }
        bool isTaskRunning() const { return isRunning; }
        void runTask(std::function<void(void)> tsk) {
            this->task = tsk;
            while (isRunning) {}
            assert(start.load(std::memory_order_seq_cst) == false);
            start.store(true, std::memory_order_seq_cst);
            std::atomic_thread_fence(std::memory_order_release);
            runCond.notify_one();
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
        while (true) {
            if ((pool[worker_idx % pool.size()]->isRunning.load(std::memory_order_seq_cst)) == false) { break; }
            if ((worker_idx != 0) && (worker_idx % pool.size() == 0)) {
                LOG_DEBUG("No enougn threads, spin finding...");
            }
            worker_idx ++;
        }
        assert(pool[worker_idx]->isRunning == false);
        // --- Run task in thread
        pool[worker_idx]->runTask(task);
    }

    ~ThreadPool() {
        // Worker automatically release resource inside destruct function, no need to stop manually
    }

};

}
