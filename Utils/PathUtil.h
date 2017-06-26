/* 
 * File:   PathUtil.h
 * Author: liugeng
 *
 * Created on 2017年6月21日, 下午4:19
 */

#ifndef PATHUTIL_H
#define PATHUTIL_H

#include <string>
#include <map>

namespace util {
	
	class Path {
	public:
		/** set then root search pathes with key */
		static void setRootWithKey(int key, const std::string& path);
		
		/** set directories under the root search pathes with key */
		static void setDirWithKey(int key, const std::string& path);
		
		/** combine root path and direcotry and filename as a full path */
		static std::string getFullPath(const std::string& filename, int dirkey = 0, int rootkey = 0);
		
		/** split the part before the last '/' */
		static std::string getDir(const std::string fullpath);
		
		/** check whether the path is exist, if not, make it */
		static void assure(const std::string& path);
		
		/** check whether the path is exist */
		static bool exists(const std::string& path);
		
	private:
		/** replace '\\' to '/' and add a '/' at last */
		static std::string convertBackslashes(const std::string& path);
		
		// root search pathes
		static std::map<int, std::string> _roots;
		// directories under the root search pathes
		static std::map<int, std::string> _dirs;
	};

}

#endif /* PATHUTIL_H */

