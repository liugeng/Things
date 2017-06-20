/* 
 * File:   LogFile.cpp
 * Author: geng
 * 
 * Created on 2017年4月27日, 下午6:14
 */

#include "LogFile.h"
#ifdef WIN32
#include <wininet.h>
#endif

LogFile::LogFile()
: _logFilePath("./")
, _fp(nullptr)
, _length(0)
, _maxLength(2*1024*1024) //2M
, _uploadIP("10.0.6.17") //my ip
, _headerInfo("")
{
}

LogFile::~LogFile() {
    if (_fp) {
        fclose(_fp);
    }
}

void LogFile::init(	const char* logFilePath,
					const char* uploadIP,
					const char* headerInfo,
					bool willUploadPreFiles)
{
	_logFilePath = logFilePath;
	_uploadIP = uploadIP;
	_headerInfo = headerInfo;
	
	if (willUploadPreFiles) {
		uploadPreFiles();
	}
}

void LogFile::setLogFilePath(const char* path) {
    _logFilePath = path;
}

void LogFile::setMaxFileLength(int n) {
    _maxLength = n;
}

void LogFile::setHeaderInfo(string info) {
    _headerInfo = info;
}

void LogFile::setUploadIP(const char* ip) {
    _uploadIP = ip;
}

void LogFile::logInfo(const char* msg) {
    log(msg, "Info");
}

void LogFile::logWarn(const char* msg) {
    log(msg, "Warn");
}

void LogFile::logError(const char* msg) {
    log(msg, "Error");
}

void LogFile::log(const char* msg, const char* des) {
    if (!_fp) {
        openLogFile("a");
    }

    static char timestr[32] = {};
    static char buf[256] = {};
    time_t t = time(0);
    strftime(timestr, sizeof(timestr), "[%m-%d %X]", localtime(&t));
    sprintf(buf, "%s[%s] %s\n", timestr, des, msg);
    
    _length += strlen(buf);
    if (_length > _maxLength) {
        openLogFile("w");
    }
    
    fwrite(buf, sizeof(c8), strlen(buf), _fp);
    fflush(_fp);
}

string LogFile::getLogFileName() {
    time_t t = time(0);
    tm* t1 = localtime(&t);
    char fileName[64] = {};
    string macAddress;
    util::str2str(util::getMacAddress(), macAddress);
    sprintf(fileName, "Err_%02d-%02d_%s.log", t1->tm_mon+1, t1->tm_mday, macAddress.c_str());
    return fileName;
}

void LogFile::openLogFile(const char* opt) {
    if (_fp) {
        fclose(_fp);
    }
    
    string filePath = joinPath(_logFilePath.c_str(), getLogFileName().c_str());
    
    _fp = fopen(filePath.c_str(), opt);
    if (!_fp) {
        return;
    }

    if (opt == "a") {
        fseek(_fp, 0L, SEEK_END);
        _length = ftell(_fp);
        if (_length >= _maxLength) {
            fclose(_fp);
            _fp = nullptr;
            openLogFile("w");
        } else if (_length == 0 && _headerInfo != "") {
            _length = fwrite(_headerInfo.c_str(), sizeof(c8), _headerInfo.length(), _fp);
        }
    } else {
        if (_headerInfo != "") {
            _length = fwrite(_headerInfo.c_str(), sizeof(c8), _headerInfo.length(), _fp);
        } else {
            _length = 0;
        }
    }
}

void LogFile::uploadPreFiles() {
#ifdef WIN32
    if (_uploadIP == "") {
        return;
    }

    _finddata_t file;
    intptr_t dir;
    if ((dir = _findfirst(joinPath(_logFilePath.c_str(), "*.log").c_str(), &file)) != -1) {
        string curlog = getLogFileName();
        HINTERNET internet = nullptr;
        HINTERNET connect = nullptr;
        
        int ret = 0;
        do {
            if (strcmp(file.name, curlog.c_str()) == 0) {
                continue;
            }

            if (!internet) {
                internet = InternetOpen("upload log", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, INTERNET_FLAG_NO_CACHE_WRITE);
                if (!internet) {
                    printf("InternetOpen failed!\n");
                    return;
                }

                connect = InternetConnect(internet, _uploadIP.c_str(), INTERNET_DEFAULT_FTP_PORT, "", "", INTERNET_SERVICE_FTP, INTERNET_FLAG_EXISTING_CONNECT || INTERNET_FLAG_PASSIVE, 0);
                if (!connect) {
                    InternetCloseHandle(internet);
                    printf("InternetConnect failed! %lu\n", GetLastError());
                    return;
                }
            }
            
            string filePath = joinPath(_logFilePath.c_str(), file.name);
            ret = FtpPutFile(connect, filePath.c_str(), file.name, FTP_TRANSFER_TYPE_ASCII, 0);
            if (ret) {
                printf("upload: %s\n", file.name);
                remove(filePath.c_str());
            } else {
                printf("FtpPutFile failed! %lu\n", GetLastError());
            }
        } while(_findnext(dir, &file) == 0);
        
        _findclose(dir);
        InternetCloseHandle(internet);
    }
#endif
}

string LogFile::joinPath(const char* path1, const char* path2) {
    string ret(path1);
    if (!ret.endsWith("/") && !ret.endsWith("\\")) {
        ret.append("/");
    }
    ret.append(path2);
    return ret;
}