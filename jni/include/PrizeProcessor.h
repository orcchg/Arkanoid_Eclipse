#ifndef __ARKANOID_PRIZE_PROCESSOR__H__
#define __ARKANOID_PRIZE_PROCESSOR__H__

#include <atomic>
#include <memory>
#include <mutex>

#include "ActiveObject.h"
#include "Bite.h"
#include "Event.h"
#include "EventListener.h"

namespace game {

/// @class PrizeProcessor PrizeProcessor.h "include/PrizeProcessor.h"
/// @brief Standalone thread performs processing catch prizes.
class PrizeProcessor : public ActiveObject {
public:
  typedef std::shared_ptr<PrizeProcessor> Ptr;

  PrizeProcessor();
  virtual ~PrizeProcessor() noexcept;

  /** @defgroup Callbacks These methods are responses of incoming events
   *  which PrizeProcessor subscribed on.
   *  @{
   */
  /// @brief Called when bite's location has changed.
  void callback_biteMoved(Bite moved_bite);
  /** @} */  // end of Callbacks group

// ----------------------------------------------
/* Public data-members */
public:
  /** @defgroup Event Outcoming events and listeners for incoming events.
   * @{
   */
  /// @brief Listens for bite location changes.
  EventListener<Bite> bite_location_listener;
  /** @} */  // end of Event group

// ----------------------------------------------
/* Private data-members */
private:
  /** @defgroup LogicData Game logic related data members.
   * @{
   */
  Bite m_bite;  //!< Physical bite's representation.
  /** @} */  // end of LogicData group

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  std::mutex m_bite_location_mutex;  //!< Sentinel for bite's center location changes.
  std::atomic_bool m_bite_location_received;  //!< New bite's center location has been received.
  /** @} */  // end of Mutex group

// ----------------------------------------------
/* Private member-functions */
private:
  /** @defgroup ActiveObject Basic thread lifecycle and operation functions.
   * @{
   */
  void onStart() override final;  //!< Right after thread has been launched.
  void onStop() override final;   //!< Right before thread has been stopped.
  /// @brief Automatic check whether this thread should continue to operate.
  /// @return Whether this thread should continue sleeping (false)
  /// or working (true).
  bool checkForWakeUp() override final;
  /// @brief Operate the data or do some job as a response of incoming
  /// outer event.
  void eventHandler() override final;
  /** @} */  // end of ActiveObject group

  /** @defgroup Processors Actions being performed by PrizeProcessor when
   *  corresponding event occurred and has been caught.
   *  @{
   */
  /// @brief Processing when bite's location has changed.
  void process_biteMoved();
  /** @} */  // end of Processors group

  /** @defgroup LogicFunc Game logic related member functions.
   * @{
   */
  //
  /** @} */  // end of LogicFunc group
};

}

#endif  // __ARKANOID_PRIZE_PROCESSOR__H__
