package com.orcchg.arkanoid.surface;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class InitActivity extends FragmentActivity {
  private static final String TAG = "Arkanoid_InitActivity";
  static final String bundleKey_dropStat = "bundleKey_dropStat";
  
  private boolean databaseExists = false;
  
  private Button mContinueGameButton;
//  private Button mNewGameButton;
//  private Button mAboutGameButton;
//  private Button mQuitGameButton;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Log.d(TAG, "onCreate");
    setContentView(R.layout.activity_init);
    
    mContinueGameButton = (Button) findViewById(R.id.continue_game_button);
//    mNewGameButton = (Button) findViewById(R.id.new_game_button);
//    mAboutGameButton = (Button) findViewById(R.id.about_game_button);
//    mQuitGameButton = (Button) findViewById(R.id.quit_game_button);
    
    databaseExists = checkDatabase();
    mContinueGameButton.setEnabled(databaseExists);
  }
  
  public void onClickContinueGame(View view) {
    Intent intent = new Intent(this, MainActivity.class);
    intent.putExtra(bundleKey_dropStat, false);
    startActivity(intent);
  }
  
  public void onClickNewGame(View view) {
    if (databaseExists) {
      warningDialog();
    } else {
      Intent intent = new Intent(this, MainActivity.class);
      intent.putExtra(bundleKey_dropStat, true);
      startActivity(intent);
    }
  }
  
  public void onClickAboutGame(View view) {
    // TODO: about
  }
  
  public void onClickQuitGame(View view) {
    finish();
  }
  
  /* Private methods */
  // --------------------------------------------------------------------------
  private boolean checkDatabase() {
    SQLiteDatabase checkDB = null;
    try {
      String path = getDatabasePath(Database.databaseName).getAbsolutePath();
      Log.i(TAG, "Checking database existence: " + path);
      checkDB = SQLiteDatabase.openDatabase(path, null, SQLiteDatabase.OPEN_READONLY);
    } catch (SQLiteException e) {
      // database doesn't exist yet
    }
    return checkDB != null;
  }
  
  private void warningDialog() {
    // TODO: localization strings
    final FragmentActivity activity = this;
    new AlertDialog.Builder(activity)
        .setTitle("New game")
        .setMessage("You will lose stored game state, are you sure want to start new game?")
        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            Intent intent = new Intent(activity, MainActivity.class);
            intent.putExtra(bundleKey_dropStat, true);
            startActivity(intent);
          }
        })
        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            // no-op
          }
        }).show();
  }
}
