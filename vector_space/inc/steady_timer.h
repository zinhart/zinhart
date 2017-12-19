#ifndef STEADY_TIMER_H
#define STEADY_TIMER_H
#include <chrono>
//#include <iostream>
template <class Clock=std::chrono::steady_clock,class TimeUnit=std::chrono::milliseconds>
class Steady_Timer
{
    public:
        typedef std::chrono::time_point<Clock> time_point;
        typedef std::chrono::duration<long double,TimeUnit> duration;
        Steady_Timer()noexcept;
        time_point start()noexcept;
        time_point end()noexcept;
        duration elapsedTime()noexcept;
        duration &&getTimeUnit()noexcept;
        void reset()noexcept;
        ~Steady_Timer()noexcept;
    private:
        time_point start_time;
        time_point end_time;
        Steady_Timer(Steady_Timer &a);
        Steady_Timer(const Steady_Timer &a);
        Steady_Timer(Steady_Timer &&a);
        Steady_Timer & operator = (Steady_Timer & a);
        Steady_Timer & operator = (Steady_Timer && a);
};
#endif // STEADY_TIMER_H
