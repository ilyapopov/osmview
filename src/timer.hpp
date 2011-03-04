#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

class Timer
{

    double _started;
    double _last;
    
public:

    Timer();
    
    double now() const;

    double time() const
    {
        return now() - _started;
    }
    double delta()
    {
        double lastlast = _last;
        _last = now();
        return _last - lastlast;
    }
};

#endif
