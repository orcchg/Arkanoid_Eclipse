package com.orcchg.arkanoid.surface;

import java.io.IOException;
import java.lang.ref.WeakReference;

import com.orcchg.arkanoid.surface.Database.DatabaseException;
import com.orcchg.arkanoid.surface.Database.GameStat;
import com.orcchg.arkanoid.surface.R;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;

public class MainActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_MainActivity";
  private static final int PLAYER_ID = 0;  // TODO: support multiple players
  private static final int INITIAL_LIVES = 3;
  private static final int INITIAL_LEVEL = 0;
  private static final int INITIAL_SCORE = 0;
  private int currentLives = INITIAL_LIVES;
  private int currentLevel = INITIAL_LEVEL;
  private int currentScore = INITIAL_SCORE;
  
  static {
    System.loadLibrary("Arkanoid");
  }
  
  private AsyncContext mAsyncContext;
  private GameSurface mSurface;
  private NativeResources mNativeResources;
  private TextView mInfoTextView;
  private TextView mCardinalityTextView;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Log.d(TAG, "onCreate");
    setContentView(R.layout.activity_main);
    mAsyncContext = new AsyncContext();
    mAsyncContext.setCoreEventListener(new CoreEventHandler(this));
    mSurface = (GameSurface) findViewById(R.id.surface_view);
    mInfoTextView = (TextView) findViewById(R.id.info_textview);
    mCardinalityTextView = (TextView) findViewById(R.id.cardinality_textview);
    
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
  }
  
  @Override
  protected void onResume() {
    Log.d(TAG, "onResume");
    mAsyncContext.start();
    mSurface.setAsyncContext(mAsyncContext);
    mAsyncContext.loadResources();
    
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    GameStat game_stat = app.DATABASE.getStat(PLAYER_ID);
    setLives(game_stat.lives);
    setLevel(game_stat.level);
    setScore(game_stat.score);
    mAsyncContext.loadLevel(Levels.get(currentLevel));
    super.onResume();
  }
  
  @Override
  protected void onPause() {
    Log.d(TAG, "onPause");
    setStat(PLAYER_ID, currentLives, currentLevel, currentScore);
    mAsyncContext.stop();
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
      case R.id.dropStat:
        ArkanoidApplication app = (ArkanoidApplication) getApplication();
        app.DATABASE.clearStat(PLAYER_ID);
        setLives(INITIAL_LIVES);
        setLevel(INITIAL_LEVEL);
        setScore(INITIAL_SCORE);
        break;
    }
    return true;
  }
  
  AsyncContext getAsyncContext() { return mAsyncContext; }
  
  GameStat getStat(long player_id) {
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    return app.DATABASE.getStat(player_id);
  }
  
  void setStat(long player_id, int lives, int level, int score) {
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    Log.i(TAG, "Stat to be stored: [" + lives + ", " + level + ", " + score + "]");
    if (!app.DATABASE.updateStat(player_id, lives, level, score)) {
      try {
        app.DATABASE.insertStat(player_id, lives, level, score);
      } catch (DatabaseException e) {
        e.printStackTrace();
      }
    }
  }
  
  void setLives(int lives) {
    currentLives = lives;
  }
  
  void setLevel(int level) {
    currentLevel = level;
  }
  
  void setScore(int score) {
    currentScore = score;
    mInfoTextView.setText(Integer.toString(score));
  }
  
  void setAngleValue(int angle) {
    mInfoTextView.setText(Integer.toString(angle));
  }
  
  void setCardinalityValue(int new_cardinality) {
    mCardinalityTextView.setText(Integer.toString(new_cardinality));
  }
  
  /* Core event listeners */
  // --------------------------------------------------------------------------
  private static class CoreEventHandler implements AsyncContext.CoreEventListener {
    private static final String TAG = "CoreEvent";
    private WeakReference<MainActivity> activityRef;
    private int currentLives = INITIAL_LIVES;
    private int currentLevel = INITIAL_LEVEL;
    private int currentScore = INITIAL_SCORE;
    
    CoreEventHandler(final MainActivity activity) {
      activityRef = new WeakReference<MainActivity>(activity);
      GameStat game_stat = activity.getStat(PLAYER_ID);
      currentLives = game_stat.lives;
      currentLevel = game_stat.level;
      currentScore = game_stat.score;
    }
    
    @Override
    public void onThrowBall() {
      Log.i(TAG, "Ball has been thrown!");
    }
    
    @Override
    public void onLostBall() {
      --currentLives;
      Log.i(TAG, "Ball has been lost! Lives rest: " + currentLives);
      MainActivity activity = activityRef.get();
      if (activity != null) {
        activity.setLives(currentLives);
      }
      onScoreUpdated(-250);  // lost ball decreases score
    }

    @Override
    public void onLevelFinished() {
      Log.i(TAG, "Level finished!");
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
    
    @Override
    public void onScoreUpdated(int score) {
      currentScore += score;
      if (currentScore < 0) {
        currentScore = 0;
      }
      final MainActivity activity = activityRef.get();
      if (activity != null) {
        activity.runOnUiThread(new Runnable() {
        @Override
        public void run() {
          activity.setScore(currentScore);
        }});
      }
    }
    
    @Override
    public void onAngleChanged(final int angle) {
//      final MainActivity activity = activityRef.get();
//      if (activity != null) {
//        activity.runOnUiThread(new Runnable() {
//          @Override
//          public void run() {
//            activity.setAngleValue(angle);
//          }});
//      }
    }
    
    @Override
    public void onCardinalityChanged(final int new_cardinality) {
      final MainActivity activity = activityRef.get();
      if (activity != null) {
        activity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            activity.setCardinalityValue(new_cardinality);
          }});
      }
    }
  }
}
