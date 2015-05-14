package com.orcchg.arkanoid.surface;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class InitActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_InitActivity";
  static final String bundleKey_dropStat = "bundleKey_dropStat";
  
//  private Button mContinueGameButton;
//  private Button mNewGameButton;
//  private Button mAboutGameButton;
//  private Button mQuitGameButton;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Log.d(TAG, "onCreate");
    setContentView(R.layout.activity_init);
    
//    mContinueGameButton = (Button) findViewById(R.id.continue_game_button);
//    mNewGameButton = (Button) findViewById(R.id.new_game_button);
//    mAboutGameButton = (Button) findViewById(R.id.about_game_button);
//    mQuitGameButton = (Button) findViewById(R.id.quit_game_button);
  }
  
  public void onClickContinueGame(View view) {
    Intent intent = new Intent(this, MainActivity.class);
    intent.putExtra(bundleKey_dropStat, false);
    startActivity(intent);
  }
  
  public void onClickNewGame(View view) {
    Intent intent = new Intent(this, MainActivity.class);
    intent.putExtra(bundleKey_dropStat, true);
    startActivity(intent);
  }
  
  public void onClickAboutGame(View view) {
    // TODO: about
  }
  
  public void onClickQuitGame(View view) {
    finish();
  }
}
