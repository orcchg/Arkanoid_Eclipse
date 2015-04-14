#ifndef __EVENTLISTENER__H__
#define __EVENTLISTENER__H__

#include <iostream>
#include <memory>
#include "logger.h"
#include <cxxabi.h>

template <typename type>
void print(const type *addr) // you wanted a pointer
{
  char * name = abi::__cxa_demangle(typeid(*addr).name(), 0, 0, NULL);
  DBG("type is: %s", name);
  free(name);
}

template <typename E>
class ListenerBinder;

template <typename E>
class EventListener  {
  friend class ListenerBinder<E>;
public:
    EventListener()
    { }

    virtual ~EventListener () {
      unbind();
    };

    EventListener(const EventListener&) = delete;
    EventListener& operator=( const EventListener& rhs ) = delete;

    EventListener& operator=(std::shared_ptr<ListenerBinder<E>> el_ptr) {
      if (isBinded()) { // unsubscribe from event
        this->unbind();
      }
      this->binder = el_ptr;
      el_ptr->eventListener = this;
      return *this;
    };

    bool isBinded() const {
      std::shared_ptr<ListenerBinder<E>> my_binder (binder.lock());
      if (my_binder)
        return true;
      else
        return false;
    }

private:
    std::weak_ptr<ListenerBinder<E>> binder;

    void unbind () {
//      print(this);
      std::shared_ptr<ListenerBinder<E>> my_binder (binder.lock());
      if (my_binder) {
//        DBG("Bind exhists");
        bool result = my_binder->unbindFromEvent();
        if (!result)
        ERR("cannot unbind from event");
      } else {
//      DBG("Bind doesn't exhist");
      }
    }
};


#endif
