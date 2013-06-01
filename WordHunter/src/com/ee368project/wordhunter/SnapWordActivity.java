package com.ee368project.wordhunter;
import com.ee368project.wordhunter.Preview;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.hardware.Camera.PictureCallback;
import android.os.Bundle;
import android.os.Environment;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;

public class SnapWordActivity extends Activity {

	private Preview mPreview;
	private LabelOnTop mLabelOnTop;
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// set window configuration
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		// setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		// Get the search word from Main Activity
		Intent intent = getIntent();
		String message = intent.getStringExtra(MainActivity.WORD_TO_SEARCH);
		// Create our Preview view and set it as the content of our activity.
		// Create our DrawOnTop view.
		mLabelOnTop = new LabelOnTop(this);
		// SnapMode: modeFlag = true
		mPreview = new Preview(this, mLabelOnTop, 1);
	
		setContentView(mPreview);
		addContentView(mLabelOnTop, new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT));
		
		// set the orientation as landscape
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.snap_word, menu);
		return true;
	}
	
	@Override
	public boolean onKeyDown(int keycode, KeyEvent event) {
		// check if the camera button is pressed
		if (keycode == KeyEvent.KEYCODE_CAMERA) {
			// if SnapMode -- why the state variable cannot be used here??
//			if (mModeFlag == 1 && mLabelOnTop.mState == 1) {
//				mLabelOnTop.mState = 0;
//			} else if (mModeFlag == 1 && mFocusFlag == false) {
//				mCamera.takePicture(mShutterCallback, mRawCallback,
//						mJpegCallback);
//			}
			mPreview.mCamera.takePicture(mPreview.mShutterCallback, mPreview.mRawCallback,
					mPreview.mJpegCallback);
			// no matter what mode, whenever the user touch the screen,
			// set the focusFlag to false, to enable refocus
			mPreview.mFocusFlag = false;
			return true;
		}
		return super.onKeyDown(keycode, event);
	}


}
