/* 
 * File:   StrUtil.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月21日, 下午12:25
 */

#include "StrUtil.h"
#include <string.h>

namespace util {
	
	using namespace std;
	
	void Str::split(const string& s, const string& seprator, vector<string>& vec) {
		char* arr = new char[s.length()+1];
		memset(arr, 0, s.length()+1);
		memcpy(arr, s.c_str(), s.length());
		arr[s.length()] = '\0';
		
		char* e = strtok(arr, seprator.c_str());
		while (e) {
			vec.push_back(string(e));
			e = strtok(nullptr, seprator.c_str());
		}
	}
	
	bool Str::startsWith(const string& s, const string& t) {
		return s.find(t) == 0;
	}
	
	bool Str::endsWith(const string& s, const string& t) {
		return s.rfind(t) == s.length() - t.length();
	}
	
}

