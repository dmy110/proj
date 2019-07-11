#ifndef _DMY_TIMER_H_
#define _DMY_TIMER_H_
#include <chrono>
namespace dmy_multiprocess_serivce
{

class Timer
{
public:
	Timer(std::chrono::duration<int> duration, std::chrono::time_point<std::chrono::steady_clock>& now_tp);
	bool operator() ();
private:
	std::chrono::time_point<std::chrono::steady_clock> _tp;
	std::chrono::duration<int> _duration;
	std::chrono::time_point<std::chrono::steady_clock>& _now_tp;
};




}

#endif