package com.ee368project.wordhunter;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends Activity {

	public final static String WORD_TO_SEARCH = "com.ee368project.wordhunter.MESSAGE";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		menu.add("Home");
		menu.add("Scan Mode");
		menu.add("Snap Mode");
		menu.add("About");
		return true;
	}

	/** Called when the user clicks the Scan button */
	public void scanWord(View view) {
		// Do something in response to button
		// Hide the window title and set full screen
		// Intent intent = new Intent(this, WordHunterActivity.class);
		// EditText editText = (EditText) findViewById(R.id.edit_message);
		// String message = editText.getText().toString();
		// intent.putExtra(EXTRA_MESSAGE, message);
		// startActivity(intent);
	}

	public void snapWord(View view) {
		// Do something in response to button
		// Hide the window title and set full screen
		Intent intent = new Intent(this, SnapWordActivity.class);
		EditText editText = (EditText) findViewById(R.id.edit_message);
		String message = editText.getText().toString();
		// putExtra: first parameter: key, second parameter: value
		intent.putExtra(WORD_TO_SEARCH, message);
		startActivity(intent);

	}

}
