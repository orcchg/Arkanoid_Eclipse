#ifndef __ACTIVE_OBJECT_MODULE__H__
#define __ACTIVE_OBJECT_MODULE__H__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <Event.h>

class ActiveObject {
public:
  ActiveObject() :
		is_detached(false),
		is_running(false),
		continueRunning(false)
	{
		modulesThread = nullptr;
	}

	virtual ~ActiveObject() {
		stop();
		delete modulesThread;
	}

	void join() {
		if(joinable())
			join_();
	}

	void detach() {
		if (modulesThread!=nullptr && is_running)
			if (!is_detached) {
		        detach_();
			}
	}

	bool joinable() const {
		if (modulesThread!=nullptr)
		  return modulesThread->joinable();
		else
          return false;
	}

	void launch() {
		if (!is_running)
		    launch_();
	}

	void restart() {
		launch();
	}

	virtual void stop() {
     this->stop_();
	}

	void stopWithDummyBool (bool dummy)
	{
		stop ();
	}

	void interrupt() {
		std::lock_guard<std::mutex> lk(wakeUpMutex);
		wakeUpCondition.notify_one();
	}

	virtual void run() {
		while(continueRunning) {
			{
			  std::unique_lock<std::mutex> ul(wakeUpMutex);
			  wakeUpCondition.wait(ul, [this](){ return this->checkForWakeUp() | !this->continueRunning; });
			}
			while (checkForWakeUp() & this->continueRunning) {
			  eventHandler();
			}
		}
	}

	void sleep(int milliseconds) {
	  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	}

	inline bool isRunning() const { return is_running; }

	Event<bool> module_has_launched_event;
	Event<bool> module_has_stopped_event;

protected:
	std::thread* modulesThread;
	std::mutex wakeUpMutex;
	std::condition_variable wakeUpCondition;
	std::atomic_bool continueRunning;
	bool is_running;
	bool is_detached;

	void launch_() {
	  if (isRunning())
	    return;
		continueRunning.store(true);
		modulesThread = new std::thread(
		  [this]() {
			this->onStart();
			this->run();
			this->onStop();
		});
		is_running = true;
	}

	void detach_() {
		modulesThread->detach();
		is_detached = true;
	}

	void join_() {
		modulesThread->join();
		is_running = false;
	}

	void stop_() {
		{
		  continueRunning.store(false);
		  std::lock_guard<std::mutex> lk(wakeUpMutex);
		  wakeUpCondition.notify_one();
		}
		join();
	}

	//@brief=called from new thread after launch is called
	virtual void onStart() {
		module_has_launched_event.notifyListeners(true);
	}

	//@brief=called from module's thread before join
	virtual void onStop() {
		module_has_stopped_event.notifyListeners(true);
	}

	virtual void onRestart() {
		module_has_launched_event.notifyListeners(true);
	}

	virtual bool checkForWakeUp() = 0;
	virtual void eventHandler() = 0;
};

#endif  // __ACTIVE_OBJECT_MODULE__H__
