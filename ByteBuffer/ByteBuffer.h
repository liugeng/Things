/* 
 * File:   ByteBuffer.h
 * Author: liugeng
 *
 * Created on 2017年6月28日, 下午12:19
 */

#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <string>

class ByteReader {
public:
	ByteReader(char* buf, int size);
	virtual ~ByteReader();
	
	bool readBool();
	char readChar();
	short readShort();
	int readInt();
	int64_t readInt64();
	float readFloat();
	std::string readString();
	
	void skip(int n);
	
private:
	char* _buf = nullptr;
	int _size = 0;
	int _pos = 0;
};

class ByteWriter {
public:
	ByteWriter();
	virtual ~ByteWriter();
	
	void writeBool(bool b);
	void writeChar(char c);
	void writeShort(short n);
	void writeInt(int n);
	void writeInt64(int64_t n);
	void writeFloat(float f);
	void writeString(std::string s);
	
	int size() { return _pos; }
	char* buffer() { return _buf; }
	
private:
	void checkCapacity(int append);
	
	char* _buf = nullptr;
	int _capacity = 0;
	int _pos = 0;
};

#endif /* BYTEBUFFER_H */

