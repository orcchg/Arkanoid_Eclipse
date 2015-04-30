package com.orcchg.arkanoid.surface;

import java.io.IOException;
import java.lang.ref.WeakReference;

import com.orcchg.arkanoid.R;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.MenuItem;

public class MainActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_MainActivity";
  private static final int INITIAL_LEVEL = 0;
  
  static {
    System.loadLibrary("Arkanoid");
  }
  
  private AsyncContext mAsyncContext;
  private GameSurface mSurface;
  private NativeResources mNativeResources;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    mAsyncContext = new AsyncContext();
    mAsyncContext.setCoreEventListener(new CoreEventHandler(this));
    mSurface = (GameSurface) findViewById(R.id.surface_view);
    mNativeResources = new NativeResources(getAssets());
    try {
      String[] resources = getAssets().list("");
      for (String resource : resources) {
        mNativeResources.read(resource);
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
    
  }
  
  @Override
  protected void onResume() {
    mAsyncContext.start();
    mSurface.setAsyncContext(mAsyncContext);
    mAsyncContext.loadLevel(Levels.get(INITIAL_LEVEL));
    super.onResume();
  }
  
  @Override
  protected void onPause() {
    mAsyncContext.stop();
    super.onPause();
  }
  
  @Override
  protected void onDestroy() {
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
        activity.mAsyncContext.loadLevel(Levels.get(currentLevel)); 
      }
    }
  }
}
