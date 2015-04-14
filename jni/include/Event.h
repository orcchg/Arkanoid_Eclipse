#pragma once

#include "EventListener.h"
#include "ListenerBinder.h"
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <list>

template <typename E>
class Event {
public:
	Event () : eventListenerId(0) {}

	virtual ~Event() {}

	typename ListenerBinder<E>::Ptr createListener(std::function<void (E)> listener){
	  typename ListenerBinder<E>::Ptr retval = std::make_shared<ListenerBinder<E>>(this, listener, eventListenerId);
	  listeners.emplace_back (retval);
	  eventListenerId++;
	  return retval;
	}

	/**
	 * create Event - EventListener binder
	 * @param f callback to EventListener event callback
	 * @param p pointer to EventListener instance
	 * @return ListenerBinder binder that should be stored in listener, if no left argument provided this binder will remain floating. Event will think it has listeners and will fire events, but listeners callbacks will not be called due to safety checks.
	 */
	template <typename P, typename Func>
	typename ListenerBinder<E>::Ptr createListener (Func f, P p)
	{
	  std::function<void(E)> func = std::bind (f, p, std::placeholders::_1);
	  return createListener(func);
	}

	void notifyListeners(E e){
	  for (auto & l : listeners){
	    l.binder_ptr->callListenerSafe(e);
	  }
	}

	bool removeListener(int id){
	  auto iter = listeners.begin();
	  while (iter != listeners.end()){
	    if (iter->getId() == id){
	      listeners.erase(iter);
	      return true;
	    }
	    iter++;
	  }
	  return false;
	}

	bool hasListeners () const {
	  return !listeners.empty();
	}

	int getListenersCount() const {
	  return listeners.size();
	}

protected:
	struct EventGlue {
	  typename ListenerBinder<E>::Ptr binder_ptr;
	  EventGlue(typename ListenerBinder<E>::Ptr binder) : binder_ptr (binder) { }
	  ~EventGlue() {
	    binder_ptr->event = nullptr;
	  }
	  inline const int& getId() const { return binder_ptr->getId(); }
	};


	std::list<EventGlue> listeners;
	int eventListenerId;
};
