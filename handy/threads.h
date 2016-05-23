#pragma once
#include <thread>
#include <atomic>
#include <list>
#include <vector>
#include <functional>
#include <limits>
#include <condition_variable>
#include <mutex>

namespace handy {
//继承 mutex 锁
template<typename T> struct SafeQueue: private std::mutex {
    static const int wait_infinite = std::numeric_limits<int>::max();
    //0 不限制队列中的任务数
    SafeQueue(size_t capacity=0): capacity_(capacity), exit_(false) {}
    //队列满则返回false
    bool push(T&& v);
    //超时则返回T()
    T pop_wait(int waitMs=wait_infinite);
    //超时返回false
    bool pop_wait(T* v, int waitMs=wait_infinite);

    size_t size();
    void exit();
    bool exited() { return exit_; }
private:
    // list 双向链表 方便增减,随机访问性不好
    std::list<T> items_;
    //条件变量
    std::condition_variable ready_;
    size_t capacity_;
    //原子变量
    std::atomic<bool> exit_;
    //等待
    void wait_ready(std::unique_lock<std::mutex>& lk, int waitMs);
};

typedef std::function<void()> Task;
extern template class SafeQueue<Task>;

struct ThreadPool {
    //创建线程池
    ThreadPool(int threads, int taskCapacity=0, bool start=true);
    ~ThreadPool();
    void start();
    ThreadPool& exit() { tasks_.exit(); return *this; }
    void join();

    //队列满返回false
    bool addTask(Task&& task);
    bool addTask(Task& task) { return addTask(Task(task)); }
    size_t taskSize() { return tasks_.size(); }
private:
    SafeQueue<Task> tasks_;
    //vector 类似 数组方便随机访问,增减不好
    std::vector<std::thread> threads_;
};

//以下为实现代码，不必关心
template<typename T> size_t SafeQueue<T>::size() {
     //上锁,std::lock_guard。与mutex配合使用，把锁放到lock_guard中时，mutex自动上锁，lock_guard析构时，同时把mutex解锁。
     // 主要用用同步,不是为了阻塞。不方便控制
    std::lock_guard<std::mutex> lk(*this);
    return items_.size();
}

template<typename T> void SafeQueue<T>::exit() {
    //本身是原子类型
    exit_ = true;
    std::lock_guard<std::mutex> lk(*this);
    ready_.notify_all();
}
//因为不是定长队列,所以插入不需要等待
template<typename T> bool SafeQueue<T>::push(T&& v) {
    std::lock_guard<std::mutex> lk(*this);
    if (exit_ || (capacity_ && items_.size() >= capacity_)) {
        return false;
    }
    items_.push_back(std::move(v));
    ready_.notify_one();
    return true;
}

template<typename T> void SafeQueue<T>::wait_ready(
    std::unique_lock<std::mutex>& lk, int waitMs)
{
    if (exit_ || !items_.empty()) {
        return;
    }
    if (waitMs == wait_infinite) {
        ready_.wait(lk, [this]{ return exit_ || !items_.empty(); });
    } else if (waitMs > 0){
        //chrono c++ 11 跨平台获取时间
        auto tp = std::chrono::steady_clock::now()
            + std::chrono::milliseconds(waitMs);

        while (ready_.wait_until(lk, tp) != std::cv_status::timeout 
            && items_.empty() && !exit_) {
        }
    }
}


//get
template<typename T> bool SafeQueue<T>::pop_wait(T* v, int waitMs) {
    //当 std::condition_variable 对象的某个 wait 函数被调用的时候，它使用 std::unique_lock(通过 std::mutex) 来锁住当前线程。
    // 当前线程会一直被阻塞，直到另外一个线程在相同的 std::condition_variable 对象上调用了 notification 函数来唤醒当前线程。
    //std::condition_variable 对象通常使用 std::unique_lock<std::mutex> 来等待
    // 提供了更好的上锁和解锁控制
    std::unique_lock<std::mutex> lk(*this);
    wait_ready(lk, waitMs);
    if (items_.empty()) {
        return false;
    }
    *v = std::move(items_.front());
    items_.pop_front();
    return true;
}

//get
template<typename T> T SafeQueue<T>::pop_wait(int waitMs) {
    std::unique_lock<std::mutex> lk(*this);
    wait_ready(lk, waitMs);
    if (items_.empty()) {
        return T();
    }
    T r = std::move(items_.front());
    items_.pop_front();
    return r;
}

}
