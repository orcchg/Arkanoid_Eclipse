#include <string>

#include "AsyncContextHelper.h"
#include "Level.h"

static JavaVM* jvm = nullptr;
static jclass String_clazz = nullptr;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;
  return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved) {}

/* init */
// ----------------------------------------------------------------------------
JNIEXPORT jlong JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_init
  (JNIEnv *jenv, jobject object) {
  auto ptr = new AsyncContextHelper(jenv, object);
  jlong descriptor = (jlong)(intptr_t) ptr;

  /* Subscribe on events incoming from outside */
  ptr->acontext->surface_received_listener = ptr->surface_received_event.createListener(&game::AsyncContext::callback_setWindow, ptr->acontext);
  ptr->acontext->shift_gesture_listener = ptr->shift_gesture_event.createListener(&game::AsyncContext::callback_shiftGamepad, ptr->acontext);
  ptr->acontext->throw_ball_listener = ptr->throw_ball_event.createListener(&game::AsyncContext::callback_throwBall, ptr->acontext);
  ptr->acontext->load_level_listener = ptr->load_level_event.createListener(&game::AsyncContext::callback_loadLevel, ptr->acontext);
  ptr->acontext->move_ball_listener = ptr->processor->move_ball_event.createListener(&game::AsyncContext::callback_moveBall, ptr->acontext);
  ptr->acontext->lost_ball_listener = ptr->processor->lost_ball_event.createListener(&game::AsyncContext::callback_lostBall, ptr->acontext);
  ptr->acontext->block_impact_listener = ptr->processor->block_impact_event.createListener(&game::AsyncContext::callback_blockImpact, ptr->acontext);
  ptr->acontext->level_finished_listener = ptr->processor->level_finished_event.createListener(&game::AsyncContext::callback_levelFinished, ptr->acontext);

  ptr->processor->load_level_listener = ptr->load_level_event.createListener(&game::GameProcessor::callback_loadLevel, ptr->processor);
  ptr->processor->throw_ball_listener = ptr->throw_ball_event.createListener(&game::GameProcessor::callback_throwBall, ptr->processor);
  ptr->processor->init_ball_position_listener = ptr->acontext->init_ball_position_event.createListener(&game::GameProcessor::callback_initBall, ptr->processor);
  ptr->processor->init_bite_listener = ptr->acontext->init_bite_event.createListener(&game::GameProcessor::callback_initBite, ptr->processor);
  ptr->processor->level_dimens_listener = ptr->acontext->level_dimens_event.createListener(&game::GameProcessor::callback_levelDimens, ptr->processor);
  ptr->processor->bite_location_listener = ptr->acontext->bite_location_event.createListener(&game::GameProcessor::callback_biteMoved, ptr->processor);

  return descriptor;
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_start
  (JNIEnv *jenv, jobject, jlong descriptor) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
  ptr->acontext->launch();
  ptr->processor->launch();
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_stop
  (JNIEnv *jenv, jobject, jlong descriptor) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
  ptr->acontext->stop();
  ptr->processor->stop();
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_destroy
  (JNIEnv *jenv, jobject, jlong descriptor) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
  delete ptr;
  ptr = nullptr;
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_setSurface
  (JNIEnv *jenv, jobject, jlong descriptor, jobject surface) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;

  if (surface == nullptr) {
    DBG("Releasing window...");
    ANativeWindow_release(ptr->window);
    ptr->window = nullptr;
  } else {
    DBG("Setting window...");
    ptr->window = ANativeWindow_fromSurface(jenv, surface);
  }

  ptr->surface_received_event.notifyListeners(ptr->window);
}

/* User actions */
// ----------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_shiftGamepad
  (JNIEnv *jenv, jobject, jlong descriptor, jfloat position) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
  ptr->shift_gesture_event.notifyListeners(position);
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_throwBall
  (JNIEnv *jenv, jobject, jlong descriptor, jfloat angle) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
  ptr->throw_ball_event.notifyListeners(angle);
}

/* Tools */
// ----------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_loadLevel
  (JNIEnv *jenv, jobject, jlong descriptor, jobjectArray in_level_Java) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;

  jsize length = jenv->GetArrayLength(in_level_Java);
  std::vector<std::string> array;
  array.reserve(length);

  for (jsize i = 0; i < length; ++i) {
    jstring java_str = (jstring) jenv->GetObjectArrayElement(in_level_Java, i);
    const char* raw_str = jenv->GetStringUTFChars(java_str, nullptr);
    array.emplace_back(raw_str);  // copy chars
    jenv->ReleaseStringUTFChars(java_str, raw_str);
  }

  auto level = game::Level::fromStringArray(array, length);
  ptr->load_level_event.notifyListeners(level);
}

JNIEXPORT jobjectArray JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_saveLevel
  (JNIEnv *jenv, jobject, jlong descriptor) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;

  game::Level::Ptr level_ptr = ptr->acontext->getCurrentLevelState();
  size_t size = level_ptr->numRows();

  jobjectArray out_level_Java =
      (jobjectArray) jenv->NewObjectArray(
          size,
          String_clazz,
          jenv->NewStringUTF(""));

  std::vector<std::string> array;
  array.reserve(level_ptr->numRows());
  level_ptr->toStringArray(&array);

  for (jsize i = 0; i < size; ++i) {
    jobject str = jenv->NewStringUTF(array[i].c_str());
    jenv->SetObjectArrayElement(out_level_Java, i, str);
  }
  return out_level_Java;
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_drop
  (JNIEnv *jenv, jobject, jlong descriptor) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
}

JNIEXPORT jint JNICALL Java_com_orcchg_arkanoid_surface_AsyncContext_getScore
  (JNIEnv *jenv, jobject, jlong descriptor) {
  AsyncContextHelper* ptr = (AsyncContextHelper*) descriptor;
}

/* Core */
// ----------------------------------------------------------------------------
AsyncContextHelper::AsyncContextHelper(JNIEnv* jenv, jobject object)
  : jenv(jenv)
  , window(nullptr) {
  acontext = std::make_shared<game::AsyncContext>(jvm);
  processor = std::make_shared<game::GameProcessor>(jvm, object);

  global_object = jenv->NewGlobalRef(object);
  jenv->DeleteLocalRef(object);
  jclass class_id = jenv->FindClass("com/orcchg/arkanoid/surface/AsyncContext");
  fireJavaEvent_lostBall_id = jenv->GetMethodID(class_id, "fireJavaEvent_lostBall", "(I)V");
  fireJavaEvent_levelFinished_id = jenv->GetMethodID(class_id, "fireJavaEvent_levelFinished", "()V");

  processor->setMasterObject(global_object);
  processor->setOnLostBallMethodID(fireJavaEvent_lostBall_id);
  processor->setOnLevelFinishedMethodID(fireJavaEvent_levelFinished_id);
}

AsyncContextHelper::~AsyncContextHelper() {
  jenv->DeleteGlobalRef(global_object);
  global_object = nullptr;
  acontext.reset();
  acontext = nullptr;
  processor.reset();
  processor = nullptr;
}
