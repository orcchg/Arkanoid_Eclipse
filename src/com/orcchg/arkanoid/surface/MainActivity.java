package com.orcchg.arkanoid.surface;

import java.lang.ref.WeakReference;

import com.orcchg.arkanoid.R;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.MenuItem;

public class MainActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_MainActivity";
  
  static {
    System.loadLibrary("Arkanoid");
  }
  
  private AsyncContext mAsyncContext;
  private GameSurface mSurface;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    mAsyncContext = new AsyncContext();
    mAsyncContext.setCoreEventListener(new CoreEventHandler(this));
    mSurface = (GameSurface) findViewById(R.id.surface_view);
  }
  
  @Override
  protected void onResume() {
    mAsyncContext.start();
    mSurface.setAsyncContext(mAsyncContext);
//    mAsyncContext.loadLevel(new String[] {"S B SS B S",
//                                          " S BBBS S ",
//                                          "B S SS B S"});
    mAsyncContext.loadLevel(new String[] {"          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "          ",
                                          "  BSSSSS  ",
                                          "  BSSBSS  ",
                                          "  BSSSSS  ",
                                          "          ",
                                          "          "});
//    mAsyncContext.loadLevel(new String[] {"          ",
//                                          "          ",
//                                          "S        S",
//                                          "B        B",
//                                          "S        S",
//                                          "B        S",
//                                          "S        B",
//                                          "S        S",
//                                          "S        S",
//                                          "B        B",
//                                          "S        S",
//                                          "B        S",
//                                          "S        B",
//                                          "S        S",
//                                          "B        B",
//                                          "S        S",
//                                          "B        S",
//                                          "S        B"});
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
        mAsyncContext.throwBall();
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
    
    CoreEventHandler(final MainActivity activity) {
      activityRef = new WeakReference<MainActivity>(activity);
    }
    
    @Override
    public void onLostBall() {
      Log.i(TAG, "Ball has been lost !");
    }

    @Override
    public void onLevelFinished() {
      Log.i(TAG, "Level finished !");
    }
  }
}
