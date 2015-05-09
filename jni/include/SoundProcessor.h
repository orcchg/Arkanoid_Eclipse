#ifndef __ARKANOID_SOUND_PROCESSOR__H__
#define __ARKANOID_SOUND_PROCESSOR__H__

#include <atomic>
#include <memory>
#include <mutex>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "ActiveObject.h"
#include "Event.h"
#include "EventListener.h"
#include "Resources.h"

namespace native {
namespace sound {

/// @class SoundProcessor SoundProcessor.h "include/SoundProcessor.h"
/// @brief Standalone thread to play sounds from sound buffers' queue.
/// http://habrahabr.ru/post/176933/
class SoundProcessor : public ActiveObject {
public:
  typedef std::shared_ptr<SoundProcessor> Ptr;

  SoundProcessor();
  virtual ~SoundProcessor() noexcept;

  /** @defgroup Callbacks These methods are responses of incoming events
   *  which SoundProcessor subscribed on.
   *  @{
   */
  //
  /** @} */  // end of Callbacks group

  /** @defgroup Resources Bind with external resources.
   * @{
   */
  /// @brief Sets the pointer to external resources.
  void setResourcesPtr(game::Resources* resources);
  /** @} */  // end of Resources group

// ----------------------------------------------
/* Public data-members */
public:
  /** @defgroup Event Outcoming events and listeners for incoming events.
   * @{
   */
  //
  /** @} */  // end of Event group

// ----------------------------------------------
/* Private data-members */
private:
  /** @defgroup Core Core data-structures for sound playback.
   * @{
   */
  int m_error_code;
  SLObjectItf m_engine;
  SLObjectItf m_mixer;
  SLEngineItf m_interface;
  /** @} */  // end of Core group

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  //
  /** @} */  // end of Mutex group

  /** @addtogroup Resources
   * @{
   */
  game::Resources* m_resources;
  /** @} */  // end of Resources group

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

  /** @defgroup Processors Actions being performed by SoundProcessor when
   *  corresponding event occurred and has been caught.
   *  @{
   */
  //
  /** @} */  // end of Processors group

  /** @defgroup CoreFunc Core-related internal functionality.
   * @{
   */
  bool init();  //!< Initializes sound processor stuff.
  void destroy();  //!< Releases sound processor stuff.
  /** @} */  // end of CoreFunc group
};

}
}

#endif  // __ARKANOID_SOUND_PROCESSOR__H__
