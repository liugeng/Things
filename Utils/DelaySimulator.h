/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DelaySimulator.h
 * Author: liugeng
 *
 * Created on 2017年6月27日, 下午2:51
 */

#ifndef DELAYSIMULATOR_H
#define DELAYSIMULATOR_H

#define SIMULATE_DELAY(__DELAY, __OPT) \
if (!DelaySimulator::isReached(__DELAY)) {\
	__OPT;\
}

class DelaySimulator {
public:
	static bool isReached(int delay) {
		static timeval tv;
		gettimeofday(&tv, nullptr);
		int64_t t1 = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
		static int64_t t0 = t1;
		if (t1 - t0 > delay) {
			t0 = t1;
			return true;
		}
		return false;
	}
};

#endif /* DELAYSIMULATOR_H */

