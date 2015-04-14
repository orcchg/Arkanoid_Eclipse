#ifndef __LISTENERBINDER__H__
#define __LISTENERBINDER__H__

#include "EventListener.h"

template <typename E>
class Event;

/**
 * ListenerBinder
 * binds Event and EventListener classes
 * Used for signaling of Event or EventListener instance end of life
 */
template<typename E>
class ListenerBinder {
  friend class Event<E>;
 public:
  typedef std::shared_ptr<ListenerBinder<E>> Ptr;
 private:
  Event<E>* event;
  std::function<void (E)> f;
  int id;
 public:
  EventListener<E>* eventListener;

  /**
   * called by Event for notifying listeners
   * this call is safe due to check for listener existence
   * @param e Event type aka package
   */
  void callListenerSafe (E e) {
    if (eventListener != nullptr) {
      f(e);
    }
  }

  /**
   * called by listener to remove its connectivity to event
   * @return true if remove successful false otherwise
   */
  bool unbindFromEvent () {
    if (event == nullptr)
      return false;

    bool result = event->removeListener(id);
    event = nullptr;
    return result;
  }

  /**
   * Get listener id
   * @return listener id
   */
  inline const int& getId() const {
    return id;
  }

// private:
  ListenerBinder (Event<E>* event_ptr, std::function<void (E)> f_in, int listener_id)
   : event (event_ptr)
   , f (f_in)
   , id (listener_id)
   , eventListener (nullptr) { }

};

#endif
