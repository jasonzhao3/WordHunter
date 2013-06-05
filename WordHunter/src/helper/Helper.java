package helper;

import java.util.List;

import android.hardware.Camera;
import android.util.Log;

/**
 * @author Yang Zhao
 * 
 */

public class Helper {
	
	static void findPictureSize(Camera.Parameters parameters, String LOG_TAG) {
		List<Camera.Size> picSizes = parameters.getSupportedPictureSizes();
		List<Camera.Size> previewSize = parameters.getSupportedPreviewSizes();
		Log.d(LOG_TAG, "hello everybody\n");
		List<String> focusModes = parameters.getSupportedFocusModes();
		
		for (Camera.Size size : picSizes) {
			Log.d(LOG_TAG, "supported picture size: height: " + size.height + " width:  " + size.width);
		}
		for (Camera.Size size : picSizes) {
			Log.d(LOG_TAG, "supported preview size: height: " + size.height + " width:  " + size.width);
		}
		for (String mode : focusModes) {
			Log.d(LOG_TAG, "supported focus modes: " + mode);
		}
		Log.d(LOG_TAG, "Picture format is " + parameters.getPictureFormat());
	}
}
