package com.ee368project.wordhunter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;

/**
 * @author Yang Zhao
 * 
 */
public class LabelOnTop extends View {
	Bitmap mBitmap;
	Paint mPaintRed;
	Paint mPaintYellow;
	String mResultString;

	

	
	//mState = 0
	int mState;


	private static final String TAG = "Android Debug Log: LabelOnTop";
	
	
	public LabelOnTop(Context context, int modeState) {
		super(context);
		Log.d(TAG, "enter onDraw constructor");
		mState = modeState;

		mPaintRed = new Paint();
		mPaintRed.setStyle(Paint.Style.FILL);
		mPaintRed.setColor(Color.RED);
		mPaintRed.setTextSize(25);
		
		mBitmap = null;
		
	
	}

	// To draw something, you need 4 basic components: A Bitmap to hold the
	// pixels, a Canvas to host the draw calls (writing into the bitmap), a
	// drawing primitive (e.g. Rect, Path, text, Bitmap), and a paint (to
	// describe the colors and styles for the drawing).
	@Override
	protected void onDraw(Canvas canvas) {
		Log.d(TAG, "enter onDraw");
		//Snap mode
		if (mState == SnapWordActivity.SNAP_MODE && mBitmap != null)
		{
			//get result image size
			Rect src = new Rect(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
			
			//get current screen size
			Rect dst=new Rect(0,0,canvas.getWidth(),canvas.getHeight());
			
			//draw the bitmap
			mPaintRed.setAlpha(255);
			canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
		} else if (mState == ScanWordActivity.SCAN_MODE && mResultString != null) {
     	

        	

	        
			
			//get result image size
			Rect src = new Rect(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
			
			//get current screen size
			Rect dst=new Rect(0,0,canvas.getWidth(),canvas.getHeight());
			
			mPaintRed.setAlpha(100);
			canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
			Log.d(TAG, "Successfully get the result string from the server!");
			Log.d(TAG, "ResultString: " + mResultString);
		}
		
	} // end onDraw method

	
}
