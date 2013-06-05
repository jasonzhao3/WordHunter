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
 * @author Yang Zhao & Shuo Liu
 * 
 */
public class LabelOnTop extends View {
	Bitmap mBitmap;
	Paint mPaintRed;
	Paint mPaintTransparent;

	int mCanvasState;
	static final int CLEAR = -1;

	private static final String TAG = "Android Debug Log: LabelOnTop";
	
	
	public LabelOnTop(Context context, int modeState) {
		super(context);
		Log.d(TAG, "enter onDraw constructor");
		mCanvasState = modeState;

		mPaintRed = new Paint();
		mPaintRed.setStyle(Paint.Style.FILL);
		mPaintRed.setColor(Color.RED);
		mPaintRed.setTextSize(25);
		
		mPaintTransparent = new Paint();
		mPaintTransparent.setAlpha(0);
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
		if (mCanvasState == SnapWordActivity.SNAP_MODE && mBitmap != null)
		{
			Log.d(TAG, "enter onDraw, Snap mode");
			//get result image size
			Rect src = new Rect(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
			//get current screen size
			Rect dst=new Rect(0,0,canvas.getWidth(),canvas.getHeight());
			//draw the bitmap
			mPaintRed.setAlpha(255);
			canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
		} else if (mCanvasState == ScanWordActivity.SCAN_MODE && mBitmap != null) {
			Log.d(TAG, "enter onDraw, Scan mode");
			//get result image size
			Rect src = new Rect(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
			//get current screen size
			Rect dst=new Rect(0,0,canvas.getWidth(),canvas.getHeight());
			mPaintRed.setAlpha(150);
			canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
		} else if (mCanvasState == HunterGameActivity.GAME_MODE && mBitmap != null) {
			Rect src = new Rect(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
			//get current screen size
			Rect dst=new Rect(0,0,canvas.getWidth(),canvas.getHeight());
			mPaintRed.setAlpha(150);
			canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
			
		} else if (mCanvasState == HunterGameActivity.GAME_RESULT_MODE && mBitmap != null) {
			Rect src = new Rect(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
			//get current screen size
			Rect dst=new Rect(0,0,canvas.getWidth(),canvas.getHeight());
			mPaintRed.setAlpha(255);
			canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
		} else if (mCanvasState == CLEAR) {
			canvas.drawPaint(mPaintTransparent);
		}
		
	} // end onDraw method

	public void setCanvasState(int canvasState) {
		mCanvasState = canvasState;
	}
	
}
