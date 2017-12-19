#include "../inc/steady_timer.h" 
template <class Clock,class TimeUnit>
Steady_Timer<Clock,TimeUnit>::Steady_Timer()noexcept {}
template <class Clock,class TimeUnit>
Steady_Timer<Clock,TimeUnit>::~Steady_Timer()noexcept {}
template <class Clock,class TimeUnit>
typename Steady_Timer<Clock,TimeUnit>::time_point Steady_Timer<Clock,TimeUnit>::start()noexcept
{

    start_time = Clock::now();
    return start_time;
}
template <class Clock,class TimeUnit>
typename Steady_Timer<Clock,TimeUnit>::time_point Steady_Timer<Clock,TimeUnit>::end()noexcept
{
    end_time = Clock::now();
    return end_time;
}
template <class Clock,class TimeUnit>
typename Steady_Timer<Clock,TimeUnit>::duration Steady_Timer<Clock,TimeUnit>::elapsedTime()noexcept
{
    duration elapsed_time = end_time - start_time;
    return elapsed_time;
}
template <class Clock,class TimeUnit>
typename Steady_Timer<Clock,TimeUnit>::duration && Steady_Timer<Clock,TimeUnit>::getTimeUnit() noexcept
{
    return duration();
}
template <class Clock,class TimeUnit>
void Steady_Timer<Clock,TimeUnit>::reset()noexcept
{

    start_time = start_time;
    end_time - end_time;
}
template <class Clock,class TimeUnit>
std::ostream & operator <<(std::ostream & os,Steady_Timer<Clock,TimeUnit> & ref)
{
    typename Steady_Timer<Clock,TimeUnit>::duration elapsed_time =  ref.elapsedTime();
    os<<"Elasped Time: "<<elapsed_time.count();
    if(std::is_same<TimeUnit,std::nano>::value)
    {
        os<<" nanoseconds\n";
    }
    else if(std::is_same<TimeUnit,std::micro>::value)
    {
        os<<" microseconds\n";
    }
    else if (std::is_same<TimeUnit,std::milli>::value)
    {
         os<<" milliseconds\n";
    }
    else if (std::is_same<TimeUnit,std::chrono::seconds>::value)
    {
         os<<" seconds\n";
    }
    else if (std::is_same<TimeUnit,std::chrono::minutes>::value)
    {
         os<<" minutes\n";
    }
    else if (std::is_same<TimeUnit,std::chrono::hours>::value)
    {
         os<<" hours\n";
    }
    return os;
}
