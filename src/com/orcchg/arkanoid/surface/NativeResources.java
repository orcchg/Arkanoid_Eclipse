package com.orcchg.arkanoid.surface;

import android.content.res.AssetManager;

public class NativeResources {
  private static final String TAG = "Arkanoid_NativeResources";
  
  private final long descriptor;
  
  NativeResources(AssetManager assets) {
    descriptor = init(assets);
  }
  
  long getPtr() { return descriptor; }
  
  /* Package API */
  // --------------------------------------------------------------------------
  boolean read(String filename) { return read(descriptor, filename); }
  void release() { release(descriptor); }
  
  /* Private methods */
  // --------------------------------------------------------------------------
  private native long init(AssetManager assets);
  private native boolean read(long descriptor, String filename);
  private native void release(long descriptor);
}
