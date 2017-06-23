/* 
 * File:   Math.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月23日, 上午11:57
 */

#include "MathUtil.h"
#include <cmath>
#include <string.h>

namespace util {
	
	using namespace std;
	
	string Math::formatNumber(int64_t n) {
		char buf[128] = {};
		snprintf(buf, 128, "%d", n);
		
		int len = strlen(buf);
		int m = std::ceil(len / 3.0f) - 1;
		if (m == 0) {
			return buf;
		}
		
		for (int i = 1; i <= m; i++) {
			memcpy(&buf[len+m-3*i-(i-1)], &buf[len-3*i], 3);
			buf[len+m-3*i-(i-1)-1] = ',';
		}
		return buf;
	}
	
}
