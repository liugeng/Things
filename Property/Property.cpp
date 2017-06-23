/* 
 * File:   Property.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月22日, 下午3:46
 */

#include "Property.h"
#include "../Utils/FileUtil.h"
#include <string.h>

using namespace std;

#define LINE_LEN 128

string Property::_emptyProp = "";
map<string, Property*> Property::_m;

void Property::create(const std::string name, const std::string filepath) {
	if (_m.find(name) != _m.end()) {
		printf("Property create repeat: %s\n", name.c_str());
		return;
	}
	_m[name] = new Property(filepath);
}

void Property::purge() {
	for (auto p : _m) {
		delete p.second;
	}
	_m.clear();
}

Property& Property::m(const std::string name) {
	return *_m[name];
}

Property::Property(std::string filepath) {
	try {
		if (!util::File::exists(filepath)) {
			_fp = fopen(filepath.c_str(), "wb");
		} else {
			_fp = fopen(filepath.c_str(), "rb+");
			load();
		}
	} catch (...) {
		if (_fp) {
			fclose(_fp);
			_fp = nullptr;
		}
		printf("prop load failed: %s\n", filepath.c_str());
	}
}

Property::~Property() {
	if (_fp) {
		fclose(_fp);
		_fp = nullptr;
	}
}

void Property::load() {
	if (!_fp) {
		return;
	}
	char line[LINE_LEN] = {};
	int pos = 0;

	while (!feof(_fp)) {
		memset(line, 0, LINE_LEN);
		fgets(line, LINE_LEN, _fp);
		int totallen = strlen(line);
		if (totallen == 0) {
			continue;
		}
		int len = totallen;
		while (len > 0) {
			if (line[len-1] == '\r' || line[len-1] == '\n') {
				line[len-1] = '\0';
				len--;
				continue;
			}
			break;
		}

		if (line[0] == ' ') {
			Line l{ "", pos, len };
			addEmptyLine(l);

		} else {
			char* key = line;
			char* val = nullptr;
			for (auto& c : line) {
				if (c == '=') {
					c = '\0';
					val = &c + 1;
				} else if (c == ' ') {
					c = '\0';
					break;
				}
			}

			Line l{ string(val), pos, len };
			_prop[string(key)] = move(l);
		}
		pos += totallen;
	}
}

void Property::addEmptyLine(const Line& el) {
	if (_emptylines.empty()) {
		_emptylines.push_back(move(el));
		return;
	}
	for (auto it = _emptylines.begin(); it != _emptylines.end(); it++) {
		if (el.length >= (*it).length) {
			_emptylines.insert(it, move(el));
			return;
		}
	}
	_emptylines.push_back(move(el));
}

void Property::set(string key, string val) {
	if (!_fp) {
		return;
	}
	if (_prop.find(key) != _prop.end()) {
		Line& l = _prop[key];
		fseek(_fp, l.pos, SEEK_SET);
		int len = key.length() + val.length() + 1;
		if (l.length < len) {
			for (int i = 0; i < l.length; i++) {
				fputc(' ', _fp);
			}
			l.val = "";
			addEmptyLine(l);
			_prop.erase(key);
		} else {
			l.val = val;
			writeProp(key, val);
			for (int i = len; i < l.length; i++) {
				fputc(' ', _fp);
			}
			fflush(_fp);
			return;
		}
	}
	
	int len = key.length() + val.length() + 1;
	for (auto it = _emptylines.begin(); it != _emptylines.end(); it++) {
		if ((*it).length >= len) {
			fseek(_fp, (*it).pos, SEEK_SET);
			writeProp(key, val);
			for (int i = len; i < (*it).length; i++) {
				fputc(' ', _fp);
			}
			fflush(_fp);
			_prop[key] = move(*it);
			_emptylines.erase(it);
			return;
		}
	}
	
	fseek(_fp, 0, SEEK_END);
	if (ftell(_fp) > 0) {
		fwrite("\n", 1, 1, _fp);
	}
	Line l{ val, ftell(_fp), len };
	_prop[key] = move(l);
	writeProp(key, val);
	fflush(_fp);
}

void Property::writeProp(const string& key, const string& val) {
	string s(key);
	s += "=";
	s += val;
	fwrite(s.c_str(), 1, s.length(), _fp);
}

const string& Property::get(string key) {
	auto it = _prop.find(key);
	if (it != _prop.end()) {
		return (*it).second.val;
	}
	return _emptyProp;
}

void Property::remove(std::string key) {
	auto it = _prop.find(key);
	if (it != _prop.end()) {
		Line& l = (*it).second;
		fseek(_fp, l.pos, SEEK_SET);
		for (int i = 0; i < l.length; i++) {
			fputc(' ', _fp);
		}
		l.val = "";
		addEmptyLine(l);
		_prop.erase(it);
	}
}