package com.ee368project.wordhunter;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

import android.app.ProgressDialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.ShutterCallback;
import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;


/**
 * @author Yang Zhao
 * 
 */
public class Preview extends SurfaceView implements SurfaceHolder.Callback {
	SurfaceHolder mHolder;
	Camera mCamera;
	LabelOnTop mLabelOnTop;

	int mModeFlag;
	boolean mFocusFlag;
	String wordToSearch;

	private Context mContext;

	ShutterCallback mShutterCallback;
	PictureCallback mRawCallback;
	PictureCallback mJpegCallback;
	Camera.AutoFocusCallback mAutoFocusCallback;

	private final String SERVERURL = "http://www.stanford.edu/~yzhao3/cgi-bin/ee368/searchWordOnCorn.php";
	// name for storing image captured by camera view
	private final static String INPUT_IMG_FILENAME = "/temp.jpg";
	private static final String TAG = "WordHunterPreviewClass";
	private static final String LOG_TAG = "Android Debug Log";

	Preview(Context context, LabelOnTop labelOnTop, int modeFlag, String message) {
		super(context);
		mContext = context;
		mLabelOnTop = labelOnTop;
		mFocusFlag = false;
		mModeFlag = modeFlag;
		wordToSearch = message;
		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed.
		mHolder = getHolder();
		mHolder.addCallback(this);
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

		// register shuttercallback
		mShutterCallback = new ShutterCallback() {
			public void onShutter() {

			}
		};

		// Handles data for raw picture
		mRawCallback = new PictureCallback() {
			@Override
			public void onPictureTaken(byte[] arg0, android.hardware.Camera arg1) {
			}
		};

		// Define jpeg callback
		mJpegCallback = new PictureCallback() {
			public void onPictureTaken(byte[] data, Camera camera) {
//				Intent mIntent = new Intent();
  				//compress image
  				compressByteImage(mContext, data, 75);  				
//  				setResult(0, mIntent);
  				
  				//** Send image and offload image processing task  to server by starting async task ** 
  				ServerTask task = new ServerTask();
  				task.execute( Environment.getExternalStorageDirectory().toString() +INPUT_IMG_FILENAME);
  				
  				//start the camera view again .
  				camera.startPreview();  
			}
		};

		// register a auto-focus call back
		mAutoFocusCallback = new Camera.AutoFocusCallback() {
			public void onAutoFocus(boolean success, Camera camera) {
				Log.d(TAG, "I'm already in the onAutoFocus callback");
				if (success == true && mFocusFlag == false)
					mFocusFlag = true;

			}
		};
	}

	
	public void surfaceCreated(SurfaceHolder holder) {
		mCamera = Camera.open();
		try {
			mCamera.setPreviewDisplay(holder);

			// Preview callback is invoked whenever new view frame is
			// available
			mCamera.setPreviewCallback(new PreviewCallback() {
				  // Called for each frame previewed
		        public void onPreviewFrame(byte[] data, Camera camera) {  // <11>
		           
		        	try {
						Thread.sleep(500);
						if (mFocusFlag == true && mModeFlag == ScanWordActivity.SCAN_MODE) {
							compressByteImage(mContext, data, 75);  				
		  						  				//** Send image and offload image processing task  to server by starting async task ** 
							ServerTask task = new ServerTask();
							task.execute( Environment.getExternalStorageDirectory().toString() +INPUT_IMG_FILENAME);
							//start the camera view again .
							camera.startPreview();  
						} else if (mModeFlag == SnapWordActivity.SNAP_MODE){
							Preview.this.invalidate();  // <12>
						}
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
		        	
		        }
		      });
		   


			// Define on touch listener
			// what is the difference between this callback function and the
			// onKeyDown function???
			this.setOnTouchListener(new OnTouchListener() {
				public boolean onTouch(View v, MotionEvent event) {
					if ( mModeFlag == SnapWordActivity.SNAP_MODE) {
						mCamera.takePicture(mShutterCallback, mRawCallback,
							mJpegCallback);
					} else if (mFocusFlag == true) {
						//do something in Scan_mode
					} else {
						mCamera.takePicture(mShutterCallback, mRawCallback,
								mJpegCallback);
						Log.d(TAG, "Cannot set up onAutoFocus");
					}
					//mFocusFlag = false;
					return false;
				}
			});

		} catch (IOException exception) {
			mCamera.release();
			mCamera = null;
		}

	}
	
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		// Now that the size is known, set up the camera parameters and begin
		// the preview.
		Camera.Parameters parameters = mCamera.getParameters();
		parameters.setPreviewSize(960, 640);
		parameters.setPictureSize(1280, 960);
		List<Camera.Size> sizes = parameters.getSupportedPictureSizes();
		Log.d(LOG_TAG, "hello everybody\n");
		
		for (Camera.Size size : sizes) {
			Log.d(LOG_TAG, "supported camerasize: height: " + size.height + " width:  " + size.width);
		}
		parameters.setPreviewFrameRate(15);
//		parameters.setSceneMode(Camera.Parameters.SCENE_MODE_NIGHT);
//		parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
		mCamera.setParameters(parameters);
		mCamera.startPreview();
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		// Surface will be destroyed when we return, so stop the preview.
		// Because the CameraDevice object is not a shared resource, it's very
		// important to release it when the activity is paused.
		mCamera.setPreviewCallback(null);
		mCamera.stopPreview();
		mCamera.release();
		mCamera = null;
	}
	
	// store the image as a jpeg image
	public boolean compressByteImage(Context mContext, byte[] imageData,
			int quality) {
		File sdCard = Environment.getExternalStorageDirectory();
		FileOutputStream fileOutputStream = null;

		try {
			BitmapFactory.Options options = new BitmapFactory.Options();
			options.inSampleSize = 1; // no downsampling
			Bitmap myImage = BitmapFactory.decodeByteArray(imageData, 0,
					imageData.length, options);
			fileOutputStream = new FileOutputStream(sdCard.toString()
					+ INPUT_IMG_FILENAME);

			BufferedOutputStream bos = new BufferedOutputStream(
					fileOutputStream);

			// compress image to jpeg
			myImage.compress(CompressFormat.JPEG, quality, bos);

			bos.flush();
			bos.close();
			fileOutputStream.close();

		} catch (FileNotFoundException e) {
			Log.e(TAG, "FileNotFoundException");
			e.printStackTrace();
		} catch (IOException e) {
			Log.e(TAG, "IOException");
			e.printStackTrace();
		}
		return true;
	}


	/**
	 * Inner class -- ServerTask, used to upload the image file to the server
	 *  
	 */
	public class ServerTask extends AsyncTask<String, Integer, Void> {
		public byte[] dataToServer;

		// Task state
		private final int UPLOADING_PHOTO_STATE = 0;
		private final int SERVER_PROC_STATE = 1;

		private ProgressDialog dialog;

		public ServerTask() {
			dialog = new ProgressDialog(mContext);
		}

		// upload photo to server
		HttpURLConnection uploadPhoto(FileInputStream fileInputStream) {

			final String serverFileName = "test"
					+ (int) Math.round(Math.random() * 1000) + ".jpg";
			final String lineEnd = "\r\n";
			final String twoHyphens = "--";
			final String boundary = "*****";

			try {
				String urlString = SERVERURL + "?word=" + wordToSearch;
				Log.d(TAG, urlString);
				URL url = new URL(urlString);
				// Open a HTTP connection to the URL
				final HttpURLConnection conn = (HttpURLConnection) url
						.openConnection();
				// Allow Inputs
				conn.setDoInput(true);
				// Allow Outputs
				conn.setDoOutput(true);
				// Don't use a cached copy.
				conn.setUseCaches(false);

				// Use a post method.
				conn.setRequestMethod("POST");
				conn.setRequestProperty("Connection", "Keep-Alive");
				conn.setRequestProperty("Content-Type",
						"multipart/form-data;boundary=" + boundary);

				DataOutputStream dos = new DataOutputStream(
						conn.getOutputStream());

				dos.writeBytes(twoHyphens + boundary + lineEnd);
				dos.writeBytes("Content-Disposition: form-data; name=\"uploadedfile\";filename=\""
						+ serverFileName + "\"" + lineEnd);
				dos.writeBytes(lineEnd);

				// create a buffer of maximum size
				int bytesAvailable = fileInputStream.available();
				int maxBufferSize = 1024;
				int bufferSize = Math.min(bytesAvailable, maxBufferSize);
				byte[] buffer = new byte[bufferSize];

				// read file and write it into form...
				int bytesRead = fileInputStream.read(buffer, 0, bufferSize);

				while (bytesRead > 0) {
					dos.write(buffer, 0, bufferSize);
					bytesAvailable = fileInputStream.available();
					bufferSize = Math.min(bytesAvailable, maxBufferSize);
					bytesRead = fileInputStream.read(buffer, 0, bufferSize);
				}

				// send multipart form data after file data...
				dos.writeBytes(lineEnd);
				dos.writeBytes(twoHyphens + boundary + twoHyphens + lineEnd);
				publishProgress(SERVER_PROC_STATE);
				// close streams
				fileInputStream.close();
				dos.flush();

				return conn;
			} catch (MalformedURLException ex) {
				Log.e(TAG, "error: " + ex.getMessage(), ex);
				return null;
			} catch (IOException ioe) {
				Log.e(TAG, "error: " + ioe.getMessage(), ioe);
				return null;
			}
		}

				
		
		// get image result from server and display it in result view
		void getResultImage(HttpURLConnection conn) {
			// retrieve the response from server
			InputStream is;
			try {
				is = conn.getInputStream();
				// get result image from server
				mLabelOnTop.mBitmap = BitmapFactory.decodeStream(is);
				is.close();
				mLabelOnTop.mState = 1; //STATE_SNAP_MODE
			} catch (IOException e) {
				Log.e(TAG, e.toString());
				e.printStackTrace();
			}
		}
		
		void getResultString(HttpURLConnection conn) {
			try {
				int rc = conn.getResponseCode();
				if (rc == 200) {
					Log.d(TAG, "response code correct");
					BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
	                StringBuilder sb = new StringBuilder();
	                mLabelOnTop.mResultString =sb.toString();
				}
				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			Log.d(TAG, "successfully get the response string in Preview");
		
//			InputStream is;
//			try {
//				is = conn.getInputStream();
//				// get result image from server
//				mLabelOnTop.mResultString = getString(is);
//				is.close();
//				mLabelOnTop.mState = 1; //STATE_SNAP_MODE
//			} catch (IOException e) {
//				Log.e(TAG, e.toString());
//				e.printStackTrace();
//			}
		}
		
		
		// Main code for processing image algorithm on the server

		void processImage(String inputImageFilePath) {
			if (mModeFlag == SnapWordActivity.SNAP_MODE)
				publishProgress(UPLOADING_PHOTO_STATE);
			
			File inputFile = new File(inputImageFilePath);
			try {

				// create file stream for captured image file
				FileInputStream fileInputStream = new FileInputStream(inputFile);

				// upload photo
				final HttpURLConnection conn = uploadPhoto(fileInputStream);

				// get processed photo from server
				if (conn != null) {
					getResultImage(conn);
					getResultString(conn);
				}
				fileInputStream.close();
			} catch (FileNotFoundException ex) {
				Log.e(TAG, ex.toString());
			} catch (IOException ex) {
				Log.e(TAG, ex.toString());
			}
		}

		protected void onPreExecute() {
			if (mModeFlag == SnapWordActivity.SNAP_MODE) {
				this.dialog.setMessage("Photo captured");
				this.dialog.show();
			}
		}
		
		

		@Override
		protected Void doInBackground(String... params) { // background
															// operation
			String uploadFilePath = params[0];
			processImage(uploadFilePath);
			// release camera when previous image is processed
			//mFocusFlag = true;
			return null;
		}

		// progress update, display dialogs
		@Override
		protected void onProgressUpdate(Integer... progress) {
			if (mModeFlag == SnapWordActivity.SNAP_MODE) {
				if (progress[0] == UPLOADING_PHOTO_STATE) {
					dialog.setMessage("Uploading");
					dialog.show();
				} else if (progress[0] == SERVER_PROC_STATE) {
					if (dialog.isShowing()) {
						dialog.dismiss();
					}
					dialog.setMessage("Processing");
					dialog.show();
				}
			}
		}

		@Override
		protected void onPostExecute(Void param) {
			if (dialog.isShowing()) {
				dialog.dismiss();
			}
		}
	}

}
