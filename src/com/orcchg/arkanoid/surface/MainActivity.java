package com.orcchg.arkanoid.surface;

import java.io.IOException;
import java.lang.ref.WeakReference;

import com.orcchg.arkanoid.R;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.MenuItem;

public class MainActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_MainActivity";
  private static final String PreferencesName = "com.orcchg.arkanoid.surface";
  private static final String bundleKey_startLevel = "bundleKey_startLevel";
  private static final int INITIAL_LEVEL = 0;
  private int mStartLevel;
  
  static {
    System.loadLibrary("Arkanoid");
  }
  
  private AsyncContext mAsyncContext;
  private GameSurface mSurface;
  private NativeResources mNativeResources;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Log.d(TAG, "onCreate");
    setContentView(R.layout.activity_main);
    mAsyncContext = new AsyncContext();
    mAsyncContext.setCoreEventListener(new CoreEventHandler(this));
    mSurface = (GameSurface) findViewById(R.id.surface_view);
    mNativeResources = new NativeResources(getAssets());
    try {
      String[] resources = getAssets().list("");
      for (String resource : resources) {
        if (!resource.equals("webkit") && !resource.equals("webkitsec") &&
            !resource.equals("sounds") && !resource.equals("images")) {
          mNativeResources.read(resource);
        }
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
    mAsyncContext.setResourcesPtr(mNativeResources.getPtr());
    
    SharedPreferences prefs = getSharedPreferences(PreferencesName, Context.MODE_PRIVATE);
    mStartLevel = prefs.getInt(bundleKey_startLevel, INITIAL_LEVEL);
  }
  
  @Override
  protected void onResume() {
    Log.d(TAG, "onResume");
    mAsyncContext.start();
    mSurface.setAsyncContext(mAsyncContext);
    mAsyncContext.loadResources();
    mAsyncContext.loadLevel(Levels.get(mStartLevel));
    super.onResume();
  }
  
  @Override
  protected void onPause() {
    Log.d(TAG, "onPause");
    mAsyncContext.stop();
    SharedPreferences prefs = getSharedPreferences("com.orcchg.arkanoid.surface", Context.MODE_PRIVATE);
    prefs.edit().putInt(bundleKey_startLevel, mStartLevel);
    super.onPause();
  }
  
  @Override
  protected void onDestroy() {
    Log.d(TAG, "onDestroy");
    mAsyncContext.destroy();
    mNativeResources.release();
    super.onDestroy();
  }
  
  @Override
  public boolean onCreateOptionsMenu(android.view.Menu menu) {
    getMenuInflater().inflate(R.menu.arkanoid_menu, menu);
    return true;
  };
  
  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.throwBall:
        mAsyncContext.throwBall((float) Math.PI / 6);
        break;
      case R.id.nextLevel:
        mAsyncContext.fireJavaEvent_levelFinished();
        break;
    }
    return true;
  }
  
  AsyncContext getAsyncContext() { return mAsyncContext; }    
  void setLevel(int level) {
    mStartLevel = level;
    Log.i(TAG, "Current level: " + mStartLevel);
  }
  
  /* Core event listeners */
  // --------------------------------------------------------------------------
  private static class CoreEventHandler implements AsyncContext.CoreEventListener {
    private static final String TAG = "CoreEvent";
    private WeakReference<MainActivity> activityRef;
    private int currentLevel = INITIAL_LEVEL;
    
    CoreEventHandler(final MainActivity activity) {
      activityRef = new WeakReference<MainActivity>(activity);
    }
    
    @Override
    public void onThrowBall() {
      Log.i(TAG, "Ball has been thrown !");
    }
    
    @Override
    public void onLostBall(int lives_rest) {
      Log.i(TAG, "Ball has been lost ! Lives rest: " + lives_rest);
    }

    @Override
    public void onLevelFinished() {
      Log.i(TAG, "Level finished !");
      MainActivity activity = activityRef.get();
      if (activity != null) {
        ++currentLevel;
        if (currentLevel >= Levels.TOTAL_LEVELS) {
          currentLevel = INITIAL_LEVEL;
        }
        try {
          Thread.sleep(250);  // let all core treads finished their jobs
          // TODO: Show win animation instead
        } catch (InterruptedException e) {
          Thread.interrupted();
          e.printStackTrace();
        }
        activity.setLevel(currentLevel);
        activity.mAsyncContext.loadLevel(Levels.get(currentLevel)); 
      }
    }
  }
}
