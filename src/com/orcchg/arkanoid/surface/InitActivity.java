package com.orcchg.arkanoid.surface;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Resources;
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
  
  private static String mAlertDialogTitle, mAboutDialogTitle;
  private static String mOKButtonLabel, mCancelButtonLabel, mCloseButtonLabel;
  private static String mWarningMessage, mAboutMessage;
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
    
    Resources res = getResources();
    mAlertDialogTitle = res.getString(R.string.new_game_button);
    mAboutDialogTitle = res.getString(R.string.about_title);
    mOKButtonLabel = res.getString(R.string.ok_button);
    mCancelButtonLabel = res.getString(R.string.cancel_button);
    mCloseButtonLabel = res.getString(R.string.close_button);
    mWarningMessage = res.getString(R.string.warning_message);
    mAboutMessage = res.getString(R.string.about_message);
    
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
    aboutDialog();
  }
  
  public void onClickQuitGame(View view) {
    finish();
  }
  
  @Override
  protected void onDestroy() {
    mContinueGameButton = null;
    System.gc();
    super.onDestroy();
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
    final FragmentActivity activity = this;
    new AlertDialog.Builder(activity)
        .setTitle(mAlertDialogTitle)
        .setMessage(mWarningMessage)
        .setPositiveButton(mOKButtonLabel, new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            Intent intent = new Intent(activity, MainActivity.class);
            intent.putExtra(bundleKey_dropStat, true);
            startActivity(intent);
          }
        })
        .setNegativeButton(mCancelButtonLabel, new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            // no-op
          }
        }).show();
  }
  
  private void aboutDialog() {
    new AlertDialog.Builder(this)
        .setTitle(mAboutDialogTitle)
        .setMessage(mAboutMessage)
        .setPositiveButton(mCloseButtonLabel, new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            // no-op
          }
        }).show();
  }
}
