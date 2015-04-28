package com.orcchg.arkanoid.surface;

import android.view.Surface;

class AsyncContext {
  private static final String TAG = "Arkanoid_AsyncContext";
  
  private final long descriptor;
  
  interface CoreEventListener {
    void onThrowBall();
    void onLostBall(int lives_rest);
    void onLevelFinished();
  }
  
  private CoreEventListener mListener;
  
  AsyncContext() {
    descriptor = init();
  }
  
  /* Package API */
  // --------------------------------------------------------------------------
  void start() { start(descriptor); }
  void stop() { stop(descriptor); }
  void destroy() { destroy(descriptor); }
  void setSurface(Surface surface) { setSurface(descriptor, surface); }
  
  /* User actions */
  void shiftGamepad(float position) { shiftGamepad(descriptor, position); }
  void throwBall(float angle) { throwBall(descriptor, angle); }

  /* Tools */
  void loadLevel(final String[] level) { loadLevel(descriptor, level); }
  
  /* Events coming from native Core */
  void setCoreEventListener(CoreEventListener listener) {
    mListener = listener;
  }
  
  void fireJavaEvent_throwBall() {
    if (mListener != null) {
      mListener.onThrowBall();
    }
  }
  
  void fireJavaEvent_lostBall(int lives_rest) {
    if (mListener != null) {
      mListener.onLostBall(lives_rest);
    }
  }
  
  void fireJavaEvent_levelFinished() {
    if (mListener != null) {
      mListener.onLevelFinished();
    }
  }
  
  /* Private methods */
  // --------------------------------------------------------------------------
  private native long init();
  private native void start(long descriptor);
  private native void stop(long descriptor);
  private native void destroy(long descriptor);
  private native void setSurface(long descriptor, Surface surface);
  
  /* User actions */
  private native void shiftGamepad(long descriptor, float position);
  private native void throwBall(long descriptor, float angle);
  
  /* Tools */
  private native void loadLevel(long descriptor, String[] in_level);
  private native String[] saveLevel(long descriptor);
  private native void drop(long descriptor);
  private native int getScore(long descriptor);
}
