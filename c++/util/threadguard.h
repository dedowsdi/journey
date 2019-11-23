#ifndef UTIL_THREADGUARD_H
#define UTIL_THREADGUARD_H

#include <thread>

class thread_guard
{

public:

    explicit thread_guard(std::thread& t):
        _t(t)
    {}

    ~thread_guard()
    {
        if(_t.joinable())
        {
            _t.join();
        }
    }

    thread_guard(const thread_guard &)=delete;
    thread_guard& operator=(const thread_guard &)=delete;

private:
    std::thread& _t;
};


// take owner ship of thread
class scoped_thread
{

public:
    explicit scoped_thread(std::thread t):
        _t(std::move(t))
    {
        if(!_t.joinable())
            throw std::logic_error("No thread");
    }

    ~scoped_thread()
    {
        _t.join();
    }

    scoped_thread(scoped_thread const&)=delete;
    scoped_thread& operator=(scoped_thread const&)=delete;

private:
    std::thread _t;
};

#endif /* UTIL_THREADGUARD_H */
