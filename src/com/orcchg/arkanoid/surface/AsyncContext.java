package com.orcchg.arkanoid.surface;

import android.view.Surface;

class AsyncContext {
  private static final String TAG = "Arkanoid_AsyncContext";
  
  private final long descriptor;
  
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
  void shiftGamepad(float distance) { shiftGamepad(descriptor, distance); }

  /* Tools */
  void loadLevel(final String[] level) { loadLevel(descriptor, level); }
  
  /* Private methods */
  // --------------------------------------------------------------------------
  private native long init();
  private native void start(long descriptor);
  private native void stop(long descriptor);
  private native void destroy(long descriptor);
  private native void setSurface(long descriptor, Surface surface);
  
  /* User actions */
  private native void shiftGamepad(long descriptor, float distance);
  private native void throwBall(long descriptor);
  
  /* Tools */
  private native void loadLevel(long descriptor, String[] in_level);
  private native String[] saveLevel(long descriptor);
  private native void drop(long descriptor);
  private native int getScore(long descriptor);
}
