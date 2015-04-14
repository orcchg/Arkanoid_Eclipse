package com.orcchg.arkanoid.surface;

import java.lang.ref.WeakReference;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class GameSurface extends SurfaceView implements SurfaceHolder.Callback {
  private static final String TAG = "Arkanoid_GameSurface";
  
  private static int mDesiredWidth = 512;   // in dp
  private static int mDesiredHeight = 512;  // in dp
  private int mWidth;
  private int mHeight;
  
  private float touchOrigin = 0.0f;
  private float touchCurrent = 0.0f;
  
  private WeakReference<AsyncContext> mAsyncContextRef;
  
  public GameSurface(Context context) {
    this(context, null);
  }
  
  public GameSurface(Context context, AttributeSet attrs) {
    this(context, attrs, 0 /* custom default style */);
  }
  
  public GameSurface(Context context, AttributeSet attrs, int defStyleAttr) {
    this(context, attrs, defStyleAttr, 0);
  }
  
  public GameSurface(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
    super(context, attrs, defStyleAttr, defStyleRes);
    getHolder().addCallback(this);
  }

  @Override
  public void surfaceCreated(SurfaceHolder holder) {
    Log.d(TAG, "Game surface created");
  }

  @Override
  public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    Log.d(TAG, "Game surface changed");
    if (mAsyncContextRef != null) {
      AsyncContext acontext = mAsyncContextRef.get();
      if (acontext != null) {
        acontext.setSurface(holder.getSurface());
      }
    }
  }

  @Override
  public void surfaceDestroyed(SurfaceHolder holder) {
    Log.d(TAG, "Game surface destroyed");
    if (mAsyncContextRef != null) {
      AsyncContext acontext = mAsyncContextRef.get();
      if (acontext != null) {
        acontext.setSurface(null);
      }
    }
  }

  @Override
  public boolean onTouchEvent(MotionEvent event) {
    switch (event.getAction()) {
      case MotionEvent.ACTION_DOWN:
        touchOrigin = event.getX() * event.getXPrecision();
        touchCurrent = touchOrigin;
        break;
      case MotionEvent.ACTION_MOVE:
        touchCurrent = event.getX() * event.getXPrecision();
        float distance = touchCurrent - touchOrigin;
        // TODO: convert distance according to screen density and pass to native
        if (mAsyncContextRef != null) {
          AsyncContext acontext = mAsyncContextRef.get();
          if (acontext != null) {
            acontext.shiftGamepad(distance * 0.0005f);
          }
        }
        break;
      case MotionEvent.ACTION_UP:
        performClick();
      case MotionEvent.ACTION_CANCEL:
        touchOrigin = 0.0f;
        touchCurrent = 0.0f;
        break;
    }
    return true;
  }
  
  @Override
  public boolean performClick() {
    return super.performClick();
  }
  
  /* Internal methods */
  // --------------------------------------------------------------------------
  void setAsyncContext(final AsyncContext acontext) {
    mAsyncContextRef = new WeakReference<AsyncContext>(acontext);
  }
  
  @Override
  protected void onDraw(Canvas canvas) {
    super.onDraw(canvas);
  }
  
  @Override
  protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    int widthMode = MeasureSpec.getMode(widthMeasureSpec);
    int widthSize = MeasureSpec.getSize(widthMeasureSpec);
    int heightMode = MeasureSpec.getMode(heightMeasureSpec);
    int heightSize = MeasureSpec.getSize(heightMeasureSpec);

    if (widthMode == MeasureSpec.EXACTLY) {
      mWidth = widthSize;
    } else if (widthMode == MeasureSpec.AT_MOST) {
      mWidth = Math.min(mDesiredWidth, widthSize);
    } else {
      mWidth = mDesiredWidth;
    }

    if (heightMode == MeasureSpec.EXACTLY) {
      mHeight = heightSize;
    } else if (heightMode == MeasureSpec.AT_MOST) {
      mHeight = Math.min(mDesiredHeight, heightSize);
    } else {
      mHeight = mDesiredHeight;
    }

    setMeasuredDimension(mWidth, mHeight);
  }
}
