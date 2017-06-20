/* 
 * File:   AsyncLoader.cpp
 * Author: liugeng
 * 
 * Created on 2017年6月19日, 下午3:38
 */

#include "AsyncLoader.h"

#pragma region "AsyncLoader"

AsyncLoader::AsyncLoader() {
	AsyncLoaderManager::addLoader(this);
}

AsyncLoader::~AsyncLoader() {
	AsyncLoaderManager::removeLoader(this);
}

void AsyncLoader::put(IAsyncTask* task) {
	_tasksLock.lock();
	_tasks.push(task);
	_tasksLock.unlock();
	
	start();
}

void AsyncLoader::start() {
	if (_waiting) {
		_waiting = false;
		_cond.notify_one();
	} else if (!_thread) {
		_thread = new std::thread(&AsyncLoader::run, this);
		_thread->detach();
	}
}

void AsyncLoader::run() {
	while (true) {
		_tasksLock.lock();
		if (_tasks.empty()) {
			_tasksLock.unlock();
			_waiting = true;
			_cond.wait(_condLock);
		} else {
			IAsyncTask* task = _tasks.front();
			_tasks.pop();
			_tasksLock.unlock();
			bool ret = task->loadAsync();
			task->staus = (ret ? IAsyncTask::SUCCESS : IAsyncTask::FAILED);
			
			_resultsLock.lock();
			_results.push_back(task);
			_hasLoadedTask = true;
			_resultsLock.unlock();
		}
	}
}

void AsyncLoader::update() {
	if (!_hasLoadedTask) {
		return;
	}
	_hasLoadedTask = false;
	
	_resultsLock.lock();
	_toHandles.assign(_results.begin(), _results.end());
	_results.clear();
	_resultsLock.unlock();
	
	for (auto task : _toHandles) {
		task->onFinish();
	}
	_toHandles.clear();
}

#pragma endregion /* AsyncLoader */

#pragma region "AsyncLoaderManager"

std::set<AsyncLoader*> AsyncLoaderManager::_loaders;

void AsyncLoaderManager::update() {
	for (auto loader : _loaders) {
		loader->update();
	}
}

void AsyncLoaderManager::addLoader(AsyncLoader* loader) {
	_loaders.insert(loader);
}

void AsyncLoaderManager::removeLoader(AsyncLoader* loader) {
	_loaders.erase(loader);
}

#pragma endregion /* AsyncLoaderManager */