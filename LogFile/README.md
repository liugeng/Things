例:
#ifdef USE_LOG_FILE
    char logHeader[512] = {};
    const char* headerFormat = ""
    "[ ID ]: %s\n"
    "[角色]: %s\n"
    "[系统]: %s\n"
    "[机型]：%s\n"
    "\n";
    sprintf(logHeader, headerFormat, "egg", "小鬼", "Windows", "烂苹果");
	LogFile::getInstance().init("./", "10.0.6.17", logHeader, true);
#endif
	
    LogInfo("loginfo");
    LogInfo("hihi");
