package com.ee368project.wordhunter;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import android.util.Log;

public class HttpImageUploader implements Runnable {

	URL connectURL;
	String params;
	String responseString;
	byte[] dataToServer;
	boolean uploadInProgress;
	boolean serverResponded;
	boolean errorOccurred;
	LabelOnTop labelOnTop;

	HttpImageUploader(String urlString, String params, LabelOnTop labelOnTop) {
		try {
			connectURL = new URL(urlString);
		} catch (Exception ex) {
			Log.i("URL FORMATION", "MALFORMATED URL");
		}
		this.params = params + "=";
		this.labelOnTop = labelOnTop;

		serverResponded = false;
		errorOccurred = false;
		uploadInProgress = false;
	}

	void doStart() {
		synchronized (this) {
			uploadInProgress = true;
			responseString = "0:";
		}

		String existingFileName = "book_spines.jpg";

		String lineEnd = "\r\n";
		String twoHyphens = "--";
		String boundary = "*****";
		String Tag = "3rd";
		try {
			uploadInProgress = true;
			serverResponded = false;
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

			// send image data
			dos.write(labelOnTop.mQueryJpegData, 0,
					labelOnTop.mQueryJpegData.length);

			// send multipart form data necesssary after file data...
			dos.writeBytes(lineEnd);
			dos.writeBytes(twoHyphens + boundary + twoHyphens + lineEnd);

			// close streams
			Log.e(Tag, "Image data is written");
			dos.flush();

			// retrieve the response from server
			InputStream is = conn.getInputStream();
			int ch;
			StringBuffer b = new StringBuffer();
			while ((ch = is.read()) != -1) {
				b.append((char) ch);
			} // while
			synchronized (this) {
				responseString = b.toString();
			}
			Log.i("Response", responseString);
			dos.close();
		} catch (MalformedURLException ex) {
			errorOccurred = true;
			Log.e(Tag, "error: " + ex.getMessage(), ex);
		}

		catch (IOException ioe) {
			errorOccurred = true;
			Log.e(Tag, "error: " + ioe.getMessage(), ioe);
		}

		synchronized (this) {
			if (errorOccurred) {
				responseString = "0:";
			}
			uploadInProgress = false;
		}
	}

	public void run() {
		doStart();
	}

}
