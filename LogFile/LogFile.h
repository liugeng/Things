/* 
 * File:   LogFile.h
 * Author: geng
 *
 * Created on 2017年4月27日, 下午6:14
 */

#ifndef LOGFILE_H
#define LOGFILE_H

#include <gstl.h>
#include "Singleton.h"


#ifdef USE_LOG_FILE
#define LogInfo(msg) LogFile::getInstance().logInfo(msg);
#define LogWarn(msg) LogFile::getInstance().logWarn(msg);
#define LogError(msg) LogFile::getInstance().logError(msg);
#else
#define LogInfo(msg)
#define LogWarn(msg)
#define LogError(msg)
#endif


class LogFile : public ssf2::Singleton<LogFile> {
public:
    LogFile();
    virtual ~LogFile();
	
	void init(	const char* logFilePath,
				const char* uploadIP,
				const char* header,
				bool uploadPreFiles);

    void setLogFilePath(const char* path);
    void setMaxFileLength(int n);
    void setHeaderInfo(string info);
    void setUploadIP(const char* ip);
    
    void logInfo(const char* msg);
    void logWarn(const char* msg);
    void logError(const char* msg);
    
    //上传今天之前的日志，上传后删除，这么做是为了降低上传的频率，每天只有一次
    void uploadPreFiles();

private:
    string getLogFileName();
    void openLogFile(const char* opt);
    void log(const char* msg, const char* des);
    string joinPath(const char* path1, const char* path2);
    
private:
    string _logFilePath;
    FILE* _fp;
    int _length;
    int _maxLength;
    string _uploadIP;
    string _headerInfo; //文件头信息，比如账号、角色名、机型等
};

#endif /* LOGFILE_H */

