/* 
 * File:   ByteBuffer.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月28日, 下午12:19
 */

#include "ByteBuffer.h"
#include <stdlib.h>
#ifdef __WIN32__
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <exception>

using namespace std;

#define HEADER_REMAIN 2
#define DEFAULT_WRITE_BUF_SIZE 16

static bool s_isBigEndian = (htonl(1) == 1);
#define CHECK_HTON_L(_N) if (!s_isBigEndian) { _N = htonl(_N); }
#define CHECK_HTON_S(_N) if (!s_isBigEndian) { _N = htons(_N); }
#define CHECK_NTOH_L(_N) if (!s_isBigEndian) { _N = ntohl(_N); }
#define CHECK_NTOH_S(_N) if (!s_isBigEndian) { _N = ntohs(_N); }

#define CHECK_RANGE(_LEN) if (_pos + _LEN > _size) { throw out_of_buf_range(); }

class out_of_buf_range : public exception {
public:
	virtual const char* what() {
		return "ByteReader: out of buf range";
	}
};
/******************************************************************************/

ByteReader::ByteReader(char* buf, int size) {
	_buf = buf;
	_size = size;
	_pos = HEADER_REMAIN;
}

ByteReader::~ByteReader() {
	_buf = nullptr;
}

bool ByteReader::readBool() {
	CHECK_RANGE(1);
	bool b;
	memcpy(&b, _buf + _pos, 1);
	_pos += 1;
	return b;
}

char ByteReader::readChar() {
	CHECK_RANGE(1);
	char c;
	memcpy(&c, _buf + _pos, 1);
	_pos += 1;
	return c;
}

short ByteReader::readShort() {
	CHECK_RANGE(2);
	short n;
	memcpy(&n, _buf + _pos, 2);
	_pos += 2;
	CHECK_NTOH_S(n);
	return n;
}

int ByteReader::readInt() {
	CHECK_RANGE(4);
	int n;
	memcpy(&n, _buf + _pos, 4);
	_pos += 4;
	CHECK_NTOH_L(n);
	return n;
}

int64_t ByteReader::readInt64() {
	CHECK_RANGE(8);
	int low;
	int64_t high;
	memcpy(&low, _buf + _pos, 4);
	_pos += 4;
	memcpy(&high, _buf + _pos, 4);
	_pos += 4;
	CHECK_NTOH_L(low);
	CHECK_NTOH_L(high);
	return (high << 32) + low;
}

float ByteReader::readFloat() {
	CHECK_RANGE(4);
	union { float f; int i; } u;
	memcpy(&u.i, _buf + _pos, 4);
	_pos += 4;
	return u.f;
}

string ByteReader::readString() {
	int len = readShort();
	CHECK_RANGE(len);
	char* tmpbuf = new char[len];
	memset(tmpbuf, 0, len);
	memcpy(tmpbuf, _buf + _pos, len);
	_pos += len;
	string s(tmpbuf, len);
	delete [] tmpbuf;
	return move(s);
}

void ByteReader::skip(int n) {
	_pos += n;
}

/******************************************************************************/

ByteWriter::ByteWriter() {
	_capacity = DEFAULT_WRITE_BUF_SIZE;
	_buf = (char*)malloc(_capacity);
	_pos = HEADER_REMAIN;
}

ByteWriter::~ByteWriter() {
	if (_buf) {
		free(_buf);
		_buf = nullptr;
	}
}

void ByteWriter::writeBool(bool b) {
	checkCapacity(1);
	memcpy(_buf + _pos, &b, 1);
	_pos += 1;
}

void ByteWriter::writeChar(char c) {
	checkCapacity(1);
	memcpy(_buf + _pos, &c, 1);
	_pos += 1;
}

void ByteWriter::writeShort(short n) {
	checkCapacity(2);
	CHECK_HTON_S(n);
	memcpy(_buf + _pos, &n, 2);
	_pos += 2;
}

void ByteWriter::writeInt(int n) {
	checkCapacity(4);
	CHECK_HTON_L(n);
	memcpy(_buf + _pos, &n, 4);
	_pos += 4;
}

void ByteWriter::writeInt64(int64_t n) {
	checkCapacity(8);
	int low = n & 0x00000000FFFFFFFF;
	int high = n >> 32;
	CHECK_HTON_L(low);
	CHECK_HTON_L(high);
	memcpy(_buf + _pos, &low, 4);
	_pos += 4;
	memcpy(_buf + _pos, &high, 4);
	_pos += 4;
}

void ByteWriter::writeFloat(float f) {
	checkCapacity(4);
	union { float f; int i; } u;
	u.f = f;
	if (((u.i & 0x7F800000) == 0x7F800000) && (u.i & 0x7FFFFF) != 0)
        u.i = 0x7FC00000;
	memcpy(_buf + _pos, &u.i, 4);
	_pos += 4;
}

void ByteWriter::writeString(std::string s) {
	writeShort(s.length());
	checkCapacity(s.length());
	memcpy(_buf + _pos, s.c_str(), s.length());
	_pos += s.length();
}

void ByteWriter::checkCapacity(int append) {
	if (_pos + append > _capacity) {
		_capacity *= 2;
		realloc(_buf, _capacity);
		checkCapacity(append);
	}
}