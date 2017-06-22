/* 
 * File:   StrUtil.h
 * Author: liugeng
 *
 * Created on 2017年6月21日, 下午12:25
 */

#ifndef STRUTIL_H
#define STRUTIL_H

#include <string>
#include <vector>

namespace util {
	class Str {
	public:
		/** split 's' with 'seprator', and save the result to 'vec' */
		static void split(const std::string& s, const std::string& seprator, std::vector<std::string>& vec);
		
		static bool startsWith(const std::string& s, const std::string& t);
		
		static bool endsWith(const std::string& s, const std::string& t);
	};
}

#endif /* STRUTIL_H */

