package com.ee368project.wordhunter;

import android.os.Bundle;
import android.app.Activity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.view.ViewGroup.LayoutParams;
import android.support.v4.app.NavUtils;
import android.annotation.TargetApi;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Build;

public class HunterGameActivity extends Activity {

	private Preview mPreview;
	private LabelOnTop mLabelOnTop;
	final static int GAME_MODE = 3;
	final static int GAME_RESULT_MODE = 4;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		// setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		// Get the search word from Main Activity
		Intent intent = getIntent();
		String message = intent.getStringExtra(MainActivity.WORD_TO_SEARCH);
		// Create our Preview view and set it as the content of our activity.
		// Create our DrawOnTop view.
		
		mLabelOnTop = new LabelOnTop(this, GAME_MODE);
		// SnapMode: modeFlag = true
		mPreview = new Preview(this, mLabelOnTop, GAME_MODE, message);
	
		setContentView(mPreview);
		addContentView(mLabelOnTop, new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT));
		
		// set the orientation as landscape
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	}
	

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.hunter_game, menu);
		return true;
	}
	
	@Override
	public boolean onKeyDown(int keycode, KeyEvent event) {
		// check if the search button or menu button is pressed 
		// two keys => increase compatibility
		//search button starts the search by taking a snapshot
		if (keycode == KeyEvent.KEYCODE_SEARCH || keycode == KeyEvent.KEYCODE_MENU) {
			mPreview.mCamera.takePicture(mPreview.mShutterCallback, mPreview.mRawCallback,
					mPreview.mJpegCallback);
			return false;
		}  else if (keycode == KeyEvent.KEYCODE_CAMERA) {
			mPreview.setOperationMode(GAME_RESULT_MODE);
			mPreview.mCamera.takePicture(mPreview.mShutterCallback, mPreview.mRawCallback,
					mPreview.mJpegCallback);
			return false;
		}
		
		return super.onKeyDown(keycode, event);
	}


}
