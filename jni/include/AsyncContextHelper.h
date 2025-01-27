#ifndef INCLUDE_ASYNCCONTEXTHELPER_H_
#define INCLUDE_ASYNCCONTEXTHELPER_H_

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_orcchg_arkanoid_surface_AsyncContext */

#ifndef _Included_com_orcchg_arkanoid_surface_AsyncContext
#define _Included_com_orcchg_arkanoid_surface_AsyncContext
#ifdef __cplusplus
extern "C" {
#endif

/* init */
// ----------------------------------------------------------------------------
/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    init
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_init
  (JNIEnv *, jobject);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    start
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_start
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    stop
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_stop
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_destroy
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_orcchg_arkanload_resources_eventoid_surface_AsyncContext
 * Method:    setSurface
 * Signature: (JLandroid/view/Surface;)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_setSurface
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    setResourcesPtr
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_setResourcesPtr
  (JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    loadResources
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_loadResources
  (JNIEnv *, jobject, jlong);

/* User actions */
// ----------------------------------------------------------------------------
/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    shiftGamepad
 * Signature: (JF)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_shiftGamepad
  (JNIEnv *, jobject, jlong, jfloat);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    throwBall
 * Signature: (JF)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_throwBall
  (JNIEnv *, jobject, jlong, jfloat);

/* Tools */
// ----------------------------------------------------------------------------
/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    loadLevel
 * Signature: (J[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_loadLevel
  (JNIEnv *, jobject, jlong, jobjectArray);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    saveLevel
 * Signature: (J)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_saveLevel
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    setBonusBlocks
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_setBonusBlocks
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    drop
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_drop
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_orcchg_arkanoid_surface_AsyncContext
 * Method:    getScore
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_getScore
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif

/* Core */
// ----------------------------------------------------------------------------
#include "AsyncContext.h"
#include "GameProcessor.h"
#include "PrizeProcessor.h"
#include "SoundProcessor.h"

/**
 * @class AsyncContextHelper AsyncContext.h "include/AsyncContext.h"
 * @brief Helper structure to wrap render thread and outer incoming events.
 */
struct AsyncContextHelper {
  /// @brief Pointer to environment for UI thread.
  JNIEnv* jenv;
  /// @brief Pointer to current object.
  jobject global_object;
  /// @brief Global reference to java.lang.tring class.
  jclass String_clazz;

  /// @brief Pointer to a windows associated with the rendering surface.
  ANativeWindow* window;

  /// @brief Shared pointer to an instance of render thread.
  game::AsyncContext::Ptr acontext;

  /// @brief Shared pointer to an instance of game logic processor thread.
  game::GameProcessor::Ptr processor;

  /// @brief Shared pointer to an instance of prize catch processor thread.
  game::PrizeProcessor::Ptr prize_processor;

  /// @brief Shared pointer to an instance of sound processor thread.
  native::sound::SoundProcessor::Ptr sound_processor;

  /** @defgroup AsyncContextEvent Events coming to render thread from outside.
   * @{
   */
  Event<ANativeWindow*> surface_received_event;  //!< When surface has been prepared.
  Event<bool> load_resources_event;  //!< Requested load resources.
  Event<float> shift_gesture_event;  //!< When user does a motion gesture.
  Event<float> throw_ball_event;  //<! When user sends a throw ball command.
  Event<game::Level::Ptr> load_level_event;  //<! When user's requested to load level.
  /** @} */  // end of AsyncContextEvent group

  jmethodID fireJavaEvent_lostBall_id;
  jmethodID fireJavaEvent_levelFinished_id;
  jmethodID fireJavaEvent_scoreUpdated_id;
  jmethodID fireJavaEvent_angleChanged_id;
  jmethodID fireJavaEvent_cardinalityChanged_id;
  jmethodID fireJavaEvent_prizeCatch_id;
  jmethodID fireJavaEvent_errorTextureLoad_id;
  jmethodID fireJavaEvent_errorSoundLoad_id;
  jmethodID fireJavaEvent_debugMessage_id;

  AsyncContextHelper(JNIEnv* jenv, jobject master_object);
  virtual ~AsyncContextHelper();
};

#endif /* INCLUDE_ASYNCCONTEXTHELPER_H_ */
