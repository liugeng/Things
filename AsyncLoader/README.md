例:
```
#include "AsyncLoader.h"
std::mutex _printLock;
class TestTask : public IAsyncTask {
public:
	TestTask(std::string fullpath) { _fullpath = fullpath; }
	virtual bool loadAsync() override {
		_printLock.lock();
		printf("loading: %s\n", _fullpath.c_str());
		_printLock.unlock();
		sleep(100);
		return true;
	}
	virtual void onFinish() override {
		std::lock_guard<std::mutex> locker(_printLock);
		printf("loaded: %s\n", _fullpath.c_str());
		delete this;
	}
};

void test() {
	AsyncLoader* loader = new AsyncLoader();
	
	char buf[128] = {};
	for (int i = 1; i <= 10; i++) {
		memset(buf, 0, 128);
		sprintf(buf, "imagetask%02d.png", i);
		TestTask* task = new TestTask(buf);
		loader->put(task);
	}
	
	while (true) {
		AsyncLoaderManager::update();
		sleep(30);
	}
}
```
