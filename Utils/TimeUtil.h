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
		/** get microsecond time from 1970-1-1 */
		static int64_t getUsec();

		/** get millisecnod time from 1970-1-1 */
		static int64_t getMsec();
		
		/** get second time from 1970-1-1 */
		static int getSec();
		
		/** get passed microsecond time from first call this function */
		static int getUsecPassed();
		
		/** get passed millisecnod time from first call this function */
		static int getMsecPassed();
		
		/** 时间的格式化字符串 
		 * timestamp 单位秒
		 * %m month  %H 24hour  %l 12hour  %M minute  %S second
		 */
		static std::string format(std::string vformat, time_t timestamp);
	};
}

/** print time cost of codes
 * eg:
 * void func() {
 *     TIME_INIT(func)
 *     do something ...
 *     TIME_PRINT
 *     do something ...
 *     TIME_PRINT
 *  }
 * >>
 * <--func--> 10
 *   <func> 13: 1
 *   <func> 15: 2
 */
#define TIME_INIT(MSG) \
const char* __msg = #MSG;\
int __t0 = util::Time::getMsecPassed();\
int __t1 = 0;\
int __t00 = __t0;

#define TIME_RESET(MSG) \
__msg = #MSG;\
__t0 = util::Time::getMsecPassed();\

#define TIME_PRINT \
__t1 = util::Time::getMsecPassed();\
__t0 = util::Time::getMsecPassed();

#define TIME_END \
__t1 = util::Time::getMsecPassed();\
printf("  (total) [%d]: %d\n", __LINE__, __t1-__t00);\

#endif /* TIMEUTIL_H */

