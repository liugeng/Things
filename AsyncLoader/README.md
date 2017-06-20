例:
```
#include "AsyncLoader.h"
std::mutex _printLock;
class ImageTask : public IAsyncTask {
public:
	// the loaded data of resource
	unsigned char* data = nullptr;
	
	ImageTask(std::string path) { fullpath = path; }
	
	virtual bool loadAsync() override {
		_printLock.lock();
		printf("loading: %s\n", fullpath.c_str());
		_printLock.unlock();
		sleep(100);
		return true;
	}
	
	virtual void onFinish() override {
		std::lock_guard<std::mutex> locker(_printLock);
		printf("loaded: %d %s\n", status, fullpath.c_str());
		delete this;
	}
};

void test() {
	AsyncLoader* loader = new AsyncLoader();
	
	char buf[128] = {};
	for (int i = 1; i <= 10; i++) {
		memset(buf, 0, 128);
		sprintf(buf, "imagetask%02d.png", i);
		ImageTask* task = new ImageTask(buf);
		loader->put(task);
	}
	
	while (true) {
		AsyncLoaderManager::update();
		sleep(30);
	}
}
```

输出:
```
loading: imagetask01.png
loading: imagetask02.png
loaded: imagetask01.png
loading: imagetask03.png
loaded: imagetask02.png
loading: imagetask04.png
loaded: imagetask03.png
loading: imagetask05.png
loaded: imagetask04.png
loading: imagetask06.png
loaded: imagetask05.png
loading: imagetask07.png
loaded: imagetask06.png
loading: imagetask08.png
loaded: imagetask07.png
loading: imagetask09.png
loaded: imagetask08.png
loading: imagetask10.png
loaded: imagetask09.png
loaded: imagetask10.png
```
