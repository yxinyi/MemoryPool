#include "MemoryPool.h"
#include <Windows.h>
#include <thread>
#include <set>

class CStopWatch
{
public:
    CStopWatch();
    void start();
    void stop();
    double getElapsedTime(); //in s
private:
    LARGE_INTEGER m_start;
    LARGE_INTEGER m_stop;
    LARGE_INTEGER m_frequency;
};

//StopWatch.cpp
CStopWatch::CStopWatch()
{
    m_start.QuadPart = 0;
    m_stop.QuadPart = 0;
    QueryPerformanceFrequency(&m_frequency);
}

void CStopWatch::start()
{
    QueryPerformanceCounter(&m_start);
}

void CStopWatch::stop()
{
    QueryPerformanceCounter(&m_stop);
}

double CStopWatch::getElapsedTime()
{
    LARGE_INTEGER time;
    time.QuadPart = m_stop.QuadPart - m_start.QuadPart;
    return (double)time.QuadPart / (double)m_frequency.QuadPart;
}



const int g_pool_size = 10000;
const int g_loop_cnt = 20000;
const int g_thread_cnt = 3;

class A {
public:
    A() {
        //printf("construct A()\n");
    };
    ~A() {
        //printf("construct ~A()\n");
    };
    char m_memory[1];
};



template<class T>
void poolNewDelFunc() {
    CStopWatch _stop_watch;
    CMemoryPool<T> _pool;
    _stop_watch.start();
    std::set<T*> _a;
    for (int _loop_cnt = 0; _loop_cnt < g_loop_cnt; ++_loop_cnt) {
        for (int _new_cnt = 0; _new_cnt < g_pool_size; _new_cnt++) {
            T* _obj = _pool.poolNew();
            _a.insert(_obj);
        }
        for (auto _a_it = _a.begin(); _a_it != _a.end(); _a_it++) {
            _pool.poolDelete(*_a_it);
        }
        _a.clear();
    }
    _stop_watch.stop();
    printf("memory pool %lf \n", _stop_watch.getElapsedTime());
}

template<class T>
void newDelFunc() {
    CStopWatch _stop_watch;
    _stop_watch.start();
    std::set<T*> _a;
    for (int _loop_cnt = 0; _loop_cnt < g_loop_cnt; ++_loop_cnt) {
        for (int _new_cnt = 0; _new_cnt < g_pool_size; _new_cnt++) {
            T* _obj = new T;
            _a.insert(_obj);
        }
        for (auto _a_it = _a.begin(); _a_it != _a.end(); _a_it++) {
            delete* _a_it;
        }
        _a.clear();
    }
    _stop_watch.stop();

    printf("default new %lf \n", _stop_watch.getElapsedTime());
}

int main()
{
    for (size_t _idx = 0; _idx < g_thread_cnt; _idx++) {
        std::thread(newDelFunc<A>).detach();
    }

    for (size_t _idx = 0; _idx < g_thread_cnt; _idx++) {
        std::thread(poolNewDelFunc<A>).detach();
    }
    for (;;);
    return 0;
}
