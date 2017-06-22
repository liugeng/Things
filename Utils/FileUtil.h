/* 
 * File:   FileUtil.h
 * Author: liugeng
 *
 * Created on 2017年6月21日, 下午2:05
 */

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>

namespace util {
	
	class Data {
		~Data() {}
	public:
		char* data = nullptr;
		int size = 0;
		void release(bool deldata = true) {
			if (deldata && data) {
				delete data;
			}
			delete this;
		}
	};

	class File {
	public:
		/** check is file exist */
		static bool exists(const std::string& filepath);

		/** load raw data, involk 'data->release()'
		 *  when don't need the 'data' any more
		 */
		static Data* loadData(const std::string& filepath);

		/** save raw data to file */
		static void saveToFile(const std::string& filepath, char* data, int size);
	};
}

#endif /* FILEUTIL_H */

