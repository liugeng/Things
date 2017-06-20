/* 
 * File:   AsyncLoader.h
 * Author: liugeng
 *
 * Created on 2017年6月19日, 下午3:38
 */

#ifndef __ASYNCLOADER_H__
#define __ASYNCLOADER_H__

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <set>

/** the base class of load Task
 */
class IAsyncTask {
public:
	// the result of loading resource
	enum { NONE, SUCCESS, FAILED } staus = NONE;
	// the fullpath of resource
	std::string _fullpath = "";
	// the loaded data of resource
	unsigned char* _data = nullptr;
	
	/** do loading things in this function, run in loading thread
	 * @return true if load success, false if failed
	 */
	virtual bool loadAsync() = 0;
	/** handle loaded resource in this function, run in main thread
	 * the memory of 'this' and '_data' need managed by user
	 */
	virtual void onFinish() = 0;
};


class AsyncLoaderManager;
/** a loader with a loading thread
 */
class AsyncLoader {
	friend AsyncLoaderManager;
public:
	AsyncLoader();
	virtual ~AsyncLoader();

	/** add a new task */
	void put(IAsyncTask* task);

private:
	/** make thread detach or awake */
	void start();
	/** the loading loop */
	void run();
	/** handle the loaded tasks */
	void update();
	
	// the working thread
	std::thread* _thread = nullptr;
	
	// let thread wait if no task
	std::condition_variable_any _cond;
	std::mutex _condLock;
	
	// tasks to load
	std::queue<IAsyncTask*> _tasks;
	std::mutex _tasksLock;
	
	// the loaded tasks
	std::vector<IAsyncTask*> _results;
	std::mutex _resultsLock;
	bool _hasLoadedTask = false;
	
	// temp vector, traverse it to handle the loaded tasks
	std::vector<IAsyncTask*> _toHandles;
	
	// is thread is waiting
	bool _waiting = false;
};


class AsyncLoaderManager  {
	friend AsyncLoader;
public:
	/** update all loaders */
	static void update();
private:
	static void addLoader(AsyncLoader* loader);
	static void removeLoader(AsyncLoader* loader);
	
	static std::set<AsyncLoader*> _loaders;
};

#endif /* __ASYNCLOADER_H__ */

