package com.ee368project.wordhunter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.View;

/**
 * @author Yang Zhao
 * 
 */
public class LabelOnTop extends View {
	Bitmap mBitmap;
	Paint mPaintRed;
	Paint mPaintYellow;
	byte[] mYUVData;
	int[] mRGBData;
	int mImageWidth, mImageHeight;
	int[] mGrayHistogram;
	double[] mGrayCDF;
	//mState = 0
	int mState;

	// Image query
	HttpFileUploader mFileUploader;
	HttpImageUploader mImageUploader;
	String mMatchTitle;
	Thread mCurrQueryThread;
	String mUploadLocation;
	String mImageFilename;
	Bitmap mQueryBitmap;
	Bitmap mFrontCoverBitmap;
	byte[] mQueryJpegData;

	static final int STATE_ORIGINAL = 0;
	static final int STATE_SNAP_MODE = 1;
	static final int STATE_SCAN_MODE = 2;

	public LabelOnTop(Context context) {
		super(context);
		
		mState = STATE_ORIGINAL;

		mPaintRed = new Paint();
		mPaintRed.setStyle(Paint.Style.FILL);
		mPaintRed.setColor(Color.RED);
		mPaintRed.setTextSize(25);


		mBitmap = null;
		mYUVData = null;
		mRGBData = null;
		mGrayHistogram = new int[256];
		mGrayCDF = new double[256];
	
	}

	// To draw something, you need 4 basic components: A Bitmap to hold the
	// pixels, a Canvas to host the draw calls (writing into the bitmap), a
	// drawing primitive (e.g. Rect, Path, text, Bitmap), and a paint (to
	// describe the colors and styles for the drawing).
	@Override
	protected void onDraw(Canvas canvas) {
		if (mBitmap != null) {
			int canvasWidth = canvas.getWidth();
			int canvasHeight = canvas.getHeight();
			int newImageWidth = 640;
			int newImageHeight = 480;
			int marginWidth = (canvasWidth - newImageWidth) / 2;

			// Draw result image
			if (mState == STATE_ORIGINAL) {
				
			}
			else if (mState == STATE_SNAP_MODE) {
				// Draw bitmap
				Rect src = new Rect(0, 0, mImageWidth, mImageHeight);
				Rect dst = new Rect(marginWidth, 0, canvasWidth - marginWidth,
						canvasHeight);
				canvas.drawBitmap(mBitmap, src, dst, mPaintRed);
			} else { //STATE_SCAN_MODE
				return;  
			}

			
			// Draw label
			String imageStateStr;
			if (mState == STATE_ORIGINAL)
				imageStateStr = "Original Image";
			else if (mState == STATE_SNAP_MODE)
				imageStateStr = "Processed Image";
			else 
				imageStateStr = "Scan Mode";
			
			canvas.drawText(imageStateStr, marginWidth + 10 - 1, 30 - 1,
					mPaintRed);
			canvas.drawText(imageStateStr, marginWidth + 10 + 1, 30 - 1,
					mPaintRed);
			canvas.drawText(imageStateStr, marginWidth + 10 + 1, 30 + 1,
					mPaintRed);
			canvas.drawText(imageStateStr, marginWidth + 10 - 1, 30 + 1,
					mPaintRed);
			canvas.drawText(imageStateStr, marginWidth + 10, 30, mPaintRed);

		} // end if statement

		super.onDraw(canvas);

	} // end onDraw method

	
}
