/* 
 * File:   PathUtil.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月21日, 下午4:19
 */

#include "PathUtil.h"
#include <string.h>
#include <io.h>

namespace util {
	
	using namespace std;
	
	map<int, string> Path::_roots;
	map<int, string> Path::_dirs;
	
	void Path::setRootWithKey(int key, const string& path) {
		_roots[key] = move(convertBackslashes(path));
	}

	void Path::setDirWithKey(int key, const string& path) {
		_dirs[key] = move(convertBackslashes(path));
	}

	string Path::getFullPath(const string& filename, int dirkey, int rootkey) {
		if (_dirs.find(dirkey) == _dirs.end() || _roots.find(rootkey) == _roots.end()) {
			return filename;
		}
		return move(_roots[rootkey] + _dirs[dirkey] + filename);
	}
	
	string Path::getDir(const string fullpath) {
		int pos = fullpath.rfind("/");
		return move(fullpath.substr(0, pos));
	}
	
	void Path::assure(const string& path) {
		string s = move(convertBackslashes(path));
		char* arr = new char[s.length()+1];
		memset(arr, 0, s.length()+1);
		memcpy(arr, s.c_str(), s.length());
		arr[s.length()] = '\0';
		
		string t("");
		char* e = strtok(arr, "/");
		while (e) {
			if (t.empty()) {
				t.append(e);
			} else {
				t.append("/");
				t.append(e);
				if (!exists(t)) {
#ifdef _WIN32
					_mkdir(t.c_str());
#else 
					mkdir(t.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
				}
			}
			e = strtok(nullptr, "/");
		}
		
		delete[] arr;
	}
	
	bool Path::exists(const string& path) {
		return access(path.c_str(), F_OK) != -1;
	}

	string Path::convertBackslashes(const string& path) {
		if (path.empty()) {
			return path;
		}
		string ret = path;
		for (auto& i : ret) {
			if (i == '\\') {
				i = '/';
			}
		}
		if (ret[ret.length()-1] != '/') {
			ret.append("/");
		}
		return move(ret);
	}
	
}