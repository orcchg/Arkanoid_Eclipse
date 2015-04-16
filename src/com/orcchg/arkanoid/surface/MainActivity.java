package com.orcchg.arkanoid.surface;

import com.orcchg.arkanoid.R;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
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
    mSurface = (GameSurface) findViewById(R.id.surface_view);
  }
  
  @Override
  protected void onResume() {
    mAsyncContext.start();
    mSurface.setAsyncContext(mAsyncContext);
    mAsyncContext.loadLevel(new String[] {"S B SS B S",
                                          " S BBBS S ",
                                          "B S SS B S"});
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
}
