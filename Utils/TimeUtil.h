/* 
 * File:   TimeUtil.h
 * Author: liugeng
 *
 * Created on 2017年6月21日, 下午12:25
 */

#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <string>

namespace util {
	
	class Time {
	public:

		/** get millisecnod time from 1970-1-1 */
		static int64_t getMillisecond();
		
		/** get second time from 1970-1-1 */
		static int getSecond();
		
		/** get passed second time from first call this function */
		static int getTimePassed();
		
		/** 时间的格式化字符串 
		 * timestamp 单位秒
		 * %m month  %H 24hour  %l 12hour  %M minute  %S second
		 */
		static std::string format(std::string vformat, time_t timestamp);
	};
}

#endif /* TIMEUTIL_H */

