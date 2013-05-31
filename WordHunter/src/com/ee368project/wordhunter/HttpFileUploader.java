package com.ee368project.wordhunter;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import android.util.Log;

public class HttpFileUploader implements Runnable {

	URL connectURL;
	String params;
	String responseString;
	String fileName;
	byte[] dataToServer;
	boolean errorOccurred;
	boolean uploadInProgress;
	boolean uploadCanceled;
	FileInputStream fileInputStream;

	HttpFileUploader(String urlString, String params, String fileName) {
		try {
			connectURL = new URL(urlString);
		} catch (Exception ex) {
			Log.i("URL FORMATION", "MALFORMATED URL");
		}
		this.params = params + "=";
		this.fileName = fileName;

		this.errorOccurred = false;
		this.uploadInProgress = false;
		this.uploadCanceled = false;

		this.fileInputStream = null;

		this.responseString = "no match";
	}

	void doStart() {
		synchronized (this) {
			responseString = "0:";
			uploadInProgress = true;
			uploadCanceled = false;
		}
		File inputFile = new File(fileName);
		try {
			fileInputStream = new FileInputStream(inputFile);
			thirdTry();
			fileInputStream.close();
		} catch (FileNotFoundException ex) {
			synchronized (this) {
				responseString = "0:";
			}
			errorOccurred = true;
			Log.e("HttpFileUploader", ex.toString());
		} catch (IOException ex) {
			synchronized (this) {
				responseString = "0:";
			}
			errorOccurred = true;
			Log.e("HttpFileUploader", ex.toString());
		}
		synchronized (this) {
			uploadInProgress = false;
		}
	}

	void doStart(FileInputStream stream) {
		synchronized (this) {
			responseString = "no match";
			uploadInProgress = true;
			uploadCanceled = false;
		}

		fileInputStream = stream;
		thirdTry();

		synchronized (this) {
			uploadInProgress = false;
		}
	}

	void thirdTry() {
		String existingFileName = "viewfinder.jpg";

		String lineEnd = "\r\n";
		String twoHyphens = "--";
		String boundary = "*****";
		String Tag = "thirdTry";
		try {
			errorOccurred = false;

			// Open a HTTP connection to the URL
			HttpURLConnection conn = (HttpURLConnection) connectURL
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

			DataOutputStream dos = new DataOutputStream(conn.getOutputStream());

			dos.writeBytes(twoHyphens + boundary + lineEnd);
			dos.writeBytes("Content-Disposition: form-data; name=\"uploadedfile\";filename=\""
					+ existingFileName + "\"" + lineEnd);
			dos.writeBytes(lineEnd);

			Log.e(Tag, "Headers are written");

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

			// send multipart form data necesssary after file data...
			dos.writeBytes(lineEnd);
			dos.writeBytes(twoHyphens + boundary + twoHyphens + lineEnd);

			// close streams
			Log.e(Tag, "File is written");
			fileInputStream.close();
			dos.flush();

			// retrieve the response from server
			InputStream is = conn.getInputStream();
			int ch;
			StringBuffer b = new StringBuffer();
			while ((ch = is.read()) != -1) {
				b.append((char) ch);
			}
			synchronized (this) {
				if (uploadCanceled)
					responseString = "no match";
				else
					responseString = b.toString();
			}
			Log.i("Response", responseString);
			dos.close();

		} catch (MalformedURLException ex) {
			errorOccurred = true;
			synchronized (this) {
				responseString = "no match";
			}
			Log.e(Tag, "error: " + ex.getMessage(), ex);
		}

		catch (IOException ioe) {
			errorOccurred = true;
			synchronized (this) {
				responseString = "no match";
			}
			Log.e(Tag, "error: " + ioe.getMessage(), ioe);
		}
	}

	public void run() {
		doStart();
	}

}
