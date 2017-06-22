/* 
 * File:   FileUtil.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月21日, 下午2:05
 */

#include "FileUtil.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace util {
	
	using namespace std;

	bool File::exists(const string& filepath) {
		return access(filepath.c_str(), F_OK) != -1;
	}

	Data* File::loadData(const string& filepath) {
		FILE* fp = fopen(filepath.c_str(), "rb");
		if (fp) {
			Data* d = new Data();

			fseek(fp, 0, SEEK_END);
			d->size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			d->data = new char[d->size];
			memset(d->data, 0, d->size);
			int rest = d->size;
			do {
				rest -= fread(&d->data[d->size - rest], 1, rest, fp);
			} while (rest > 0);
			fclose(fp);
			return d;
		}
		return nullptr;
	}

	void File::saveToFile(const string& filepath, char* data, int size) {
		if (!data || size <= 0 || filepath.empty()) {
			return;
		}
		FILE* fp = fopen(filepath.c_str(), "wb");
		if (fp) {
			int rest = size;
			do {
				rest -= fwrite(&data[size - rest], 1, rest, fp);
			} while (rest > 0);
			fclose(fp);
		}
	}
	
}