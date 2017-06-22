/* 
 * File:   TimeUtil.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月21日, 下午12:25
 */

#include <winsock2.h>
#include <sys/time.h>
#include <time.h>
#include "TimeUtil.h"

namespace util {
	
	using namespace std;

	int64_t Time::getMillisecond() {
		static timeval tv;
		gettimeofday(&tv, nullptr);
		return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
	
	int Time::getSecond() {
		static timeval tv;
		gettimeofday(&tv, nullptr);
		return tv.tv_sec + tv.tv_usec / 1000 / 1000;
	}

	int Time::getTimePassed() {
		int64_t t1 = getMillisecond();
		static int64_t t0 = t1;
		return t1 - t0;
	}
	
	string Time::format(string vformat, time_t timestamp) {
		char buf[128] = {};
		strftime(buf, sizeof(buf), vformat.c_str(), localtime(&timestamp));
		return buf;
	}
}
