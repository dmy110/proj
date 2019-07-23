#include "timer.h"

using namespace dmy_common;

Timer::Timer(std::chrono::duration<int> duration, 
	std::chrono::time_point<std::chrono::steady_clock>& now_tp):
	_duration(duration),
	_now_tp(now_tp)
{
	_tp = now_tp;
}

bool Timer::operator() ()
{
	if (_now_tp - _tp >= _duration) {
		_tp = _now_tp;
		return true;
	}
	return false;
}
