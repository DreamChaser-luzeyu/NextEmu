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
        std::atomic<bool> isRunning;
        bool start;
        bool exit;
        std::shared_ptr<std::thread> thread;
        std::mutex runMutex;
        std::condition_variable runCond;
        std::function<void(void)> task;
        Worker() : isRunning(false), start(false), exit(false) {
            thread = std::make_shared<std::thread>([this]() {
                while (true) {
                    std::unique_lock lock(runMutex);
                    runCond.wait(lock, [this]() { return start || exit; });
                    if (exit) { break; }
                    isRunning = true;
                    task();
                    start = false;
                    isRunning = false;
                }
            });
        }
        bool isTaskRunning() const { return isRunning; }
        void runTask(std::function<void(void)> tsk) {
            this->task = tsk;
            while (isRunning) {}
            start = true;
            runCond.notify_one();
        }
        ~Worker() {
            exit = true;
            runCond.notify_all();
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
        size_t worker_idx = 0;
        while (true) {
            if ((pool[worker_idx % pool.size()]->isRunning) == false) { break; }
            worker_idx ++;
        }
        assert(pool[worker_idx]->isRunning == false);
        pool[worker_idx]->runTask(task);
    }

};

}
