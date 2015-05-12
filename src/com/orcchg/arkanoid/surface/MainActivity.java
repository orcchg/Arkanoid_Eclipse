package com.orcchg.arkanoid.surface;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Arrays;

import com.orcchg.arkanoid.surface.Database.DatabaseException;
import com.orcchg.arkanoid.surface.Database.GameStat;
import com.orcchg.arkanoid.surface.R;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.DecelerateInterpolator;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_MainActivity";
  private static final int PLAYER_ID = 1;
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
  private TextView mInfoTextView, mAddInfoTextView;
  private TextView mLifeMultiplierTextView, mCardinalityTextView;
  private ImageView[] mLifeViews;
  private AnimationSet mAnimations;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Log.d(TAG, "onCreate");
    setContentView(R.layout.activity_main);
    mAsyncContext = new AsyncContext();
    mAsyncContext.setCoreEventListener(new CoreEventHandler(this));

    mSurface = (GameSurface) findViewById(R.id.surface_view);
    mInfoTextView = (TextView) findViewById(R.id.info_textview);
    mAddInfoTextView = (TextView) findViewById(R.id.add_info_textview);
    mLifeMultiplierTextView = (TextView) findViewById(R.id.life_multiplier_textview);
    mCardinalityTextView = (TextView) findViewById(R.id.cardinality_textview);
    mLifeViews = new ImageView[] {
      (ImageView) findViewById(R.id.life1_imageview),
      (ImageView) findViewById(R.id.life2_imageview),
      (ImageView) findViewById(R.id.life3_imageview),
      (ImageView) findViewById(R.id.life4_imageview),
      (ImageView) findViewById(R.id.life5_imageview)};
    
    // ------------------------------------------
    Animation fadeIn = new AlphaAnimation(0, 1);
    fadeIn.setInterpolator(new DecelerateInterpolator());
    fadeIn.setDuration(250);
    Animation fadeOut = new AlphaAnimation(1, 0);
    fadeOut.setInterpolator(new AccelerateInterpolator());
    fadeOut.setStartOffset(250);
    fadeOut.setDuration(250);
    mAnimations = new AnimationSet(false);
    mAnimations.addAnimation(fadeIn);
    mAnimations.addAnimation(fadeOut);
    
    // ------------------------------------------
    mNativeResources = new NativeResources(getAssets(), getFilesDir().getAbsolutePath());
    try {
      String[] texture_resources = getAssets().list("texture");
      for (String texture : texture_resources) {
        mNativeResources.readTexture(texture);
      }
      String[] sound_resources = getAssets().list("sound");
      for (String sound : sound_resources) {
        mNativeResources.readSound(sound);
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
    mAsyncContext.setResourcesPtr(mNativeResources.getPtr());
    
    // ------------------------------------------
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    if (!app.DATABASE.updatePlayer(PLAYER_ID, "Player")) {
      try {
        app.DATABASE.insertPlayer("Player");
      } catch (DatabaseException e) {
        e.printStackTrace();
      }
    }
  }
  
  @Override
  protected void onResume() {
    Log.d(TAG, "onResume");
    mAsyncContext.start();
    mSurface.setAsyncContext(mAsyncContext);
    mAsyncContext.loadResources();
    
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    GameStat game_stat = app.DATABASE.getStat(PLAYER_ID);
    String level_state = "";
    if (game_stat != null) {
      setLives(game_stat.lives);
      setLevel(game_stat.level);
      setScore(game_stat.score);
      level_state = game_stat.state;
    } else {
      setLives(INITIAL_LIVES);
      setLevel(INITIAL_LEVEL);
      setScore(INITIAL_SCORE);
    }
    mAsyncContext.loadLevel(Levels.get(currentLevel, level_state));
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
        mAsyncContext.fireJavaEvent_onScoreUpdated(-10 * (int) Math.pow(currentLevel + 1, 2));
        break;
      case R.id.dropStat:
        ArkanoidApplication app = (ArkanoidApplication) getApplication();
        app.DATABASE.clearStat(PLAYER_ID);
        setLives(INITIAL_LIVES);
        setLevel(INITIAL_LEVEL);
        setScore(INITIAL_SCORE);
        mAsyncContext.fireJavaEvent_refreshLives();
        mAsyncContext.fireJavaEvent_refreshLevel();
        mAsyncContext.fireJavaEvent_refreshScore();
        mAsyncContext.loadLevel(Levels.get(currentLevel));
        break;
    }
    return true;
  }
  
  /* Gameplay methods */
  // --------------------------------------------------------------------------
  void lostGame() {
    Log.i(TAG, "Game is lost!");
    setLives(INITIAL_LIVES);
    mAsyncContext.fireJavaEvent_refreshLives();
    mAsyncContext.loadLevel(Levels.get(currentLevel, ""));
  }
  
  /* Support methods */
  // --------------------------------------------------------------------------
  AsyncContext getAsyncContext() { return mAsyncContext; }
  
  GameStat getStat(long player_id) {
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    return app.DATABASE.getStat(player_id);
  }
  
  void setStat(long player_id, int lives, int level, int score) {
    ArkanoidApplication app = (ArkanoidApplication) getApplication();
    Log.i(TAG, "Stat to be stored: [" + lives + ", " + level + ", " + score + "]");
    String levelState = mAsyncContext.saveLevel();
    Log.i(TAG, "Level: " + levelState);
    if (!app.DATABASE.updateStat(player_id, lives, level, score, levelState)) {
      try {
        app.DATABASE.insertStat(player_id, lives, level, score, levelState);
      } catch (DatabaseException e) {
        e.printStackTrace();
      }
    }
  }
  
  void setLives(int lives) {
    if (lives > mLifeViews.length) {
      mLifeMultiplierTextView.setText("+" + Integer.toString(lives - mLifeViews.length));
      lives = mLifeViews.length;
    } else if (lives < 0) {
      lives = 0;
      lostGame();
      return;
    } else {
      mLifeMultiplierTextView.setText("");
    }
    for (int i = 0; i < lives; ++i) {
      mLifeViews[i].setVisibility(View.VISIBLE);
    }
    for (int i = lives; i < mLifeViews.length; ++i) {
      mLifeViews[i].setVisibility(View.INVISIBLE);
    }
    currentLives = lives;
  }
  
  void setLevel(int level) {
    currentLevel = level;
  }
  
  void setScore(int score) {
    currentScore = score;
    mInfoTextView.setText(Integer.toString(score));
  }
  
  void setScoreUpdate(int score) {
    if (score >= 0) {
      mAddInfoTextView.setTextColor(Color.GREEN);
      mAddInfoTextView.setText("+" + Integer.toString(score));
    } else {
      mAddInfoTextView.setTextColor(Color.RED);
      mAddInfoTextView.setText(Integer.toString(score));
    }
  }
  
  void setAngleValue(int angle) {
    mInfoTextView.setText(Integer.toString(angle));
  }
  
  void setCardinalityValue(int new_cardinality) {
    mCardinalityTextView.setText(Integer.toString(new_cardinality));
  }
  
  void levelTransitionEffect() {
    mSurface.startAnimation(mAnimations);
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
      if (game_stat != null) {
        currentLives = game_stat.lives;
        currentLevel = game_stat.level;
        currentScore = game_stat.score;
      }
    }
    
    @Override
    public void onRefreshLives() {
      currentLives = INITIAL_LIVES;
    }
    
    @Override
    public void onRefreshLevel() {
      currentLevel = INITIAL_LEVEL;
    }
    
    @Override
    public void onRefreshScore() {
      currentScore = INITIAL_SCORE;
    }
    
    @Override
    public void onThrowBall() {
      Log.i(TAG, "Ball has been thrown!");
    }
    
    @Override
    public void onLostBall() {
      --currentLives;
      Log.i(TAG, "Ball has been lost! Lives rest: " + currentLives);
      updateLives();
      onScoreUpdated(-2 * (int) Math.pow(currentLevel + 1, 2));  // lost ball decreases score
    }

    @Override
    public void onLevelFinished() {
      Log.i(TAG, "Level finished!");
      final MainActivity activity = activityRef.get();
      if (activity != null) {
        ++currentLevel;
        if (currentLevel >= Levels.TOTAL_LEVELS) {
          currentLevel = INITIAL_LEVEL;
        }
        activity.setLevel(currentLevel);
        activity.mAsyncContext.loadLevel(Levels.get(currentLevel)); 
      }
    }
    
    @Override
    public void onScoreUpdated(final int score) {
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
          activity.setScoreUpdate(score);
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
    
    @Override
    public void onPrizeCatch(final Prize prize) {
      int score = 0;
      switch (prize) {
        case BLOCK:
        case CLIMB:
          score += 15;
          break;
        case DESTROY:
          --currentLives;
          updateLives();
          break;
        case DRAGON:
        case EASY:
        case EASY_T:
        case EVAPORATE:
        case EXPLODE:
        case EXTEND:
        case FAST:
        case FOG:
        case GOO:
        case HYPER:
        case INIT:
        case JUMP:
        case LASER:
        case MIRROR:
        case PIERCE:
        case PROTECT:
        case RANDOM:
        case SHORT:
        case SLOW:
        case UPGRADE:
        case DEGRADE:
          score += 15;
          break;
        case VITALITY:
          ++currentLives;
          score += 25;
          updateLives();
          break;
        case WIN:
          score += 100;
          onLevelFinished();
          break;
        case ZYGOTE:
          break;
        case SCORE_1:
          score += 35;
        case SCORE_2:
          score += 25;
        case SCORE_3:
          score += 20;
        case SCORE_4:
          score += 15;
        case SCORE_5:
          score += 10;
        default:
          break;
      }
      onScoreUpdated(score);
    }
    
    // ------------------------------------------
    private void updateLives() {
      final MainActivity activity = activityRef.get();
      if (activity != null) {
        activity.runOnUiThread(new Runnable() {
          @Override
          public void run() {
            activity.setLives(currentLives);
          }
        });
      }
    }
  }
}
