#ifndef __CONCURRENT_STR_QUEUE__
#define __CONCURRENT_STR_QUEUE__

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>


namespace CoQuiAAS {

class ConcurrentStrQueue {

public:

    inline void push(std::string str) {
        std::unique_lock<std::mutex> mlock(queue_mut);
        elements.push(str);
        mlock.unlock();
        cond.notify_one();
    }

    inline std::string pop() {
        std::unique_lock<std::mutex> mlock(queue_mut);
        while (elements.empty()){
            cond.wait(mlock);
        }
        auto item = elements.front();
        elements.pop();
        return item;
    }

private:

    std::queue<std::string> elements;

    std::mutex queue_mut;

    std::condition_variable cond;

};

}

#endif