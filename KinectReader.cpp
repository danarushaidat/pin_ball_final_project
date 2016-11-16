#include "KinectReader.h"

KinectReader::KinectReader(vector<GLubyte> pColorData, vector<GLubyte> pDepthData)
{
	// Kinect state
	kinect = NULL;
	colorStream = INVALID_HANDLE_VALUE;
	depthStream = INVALID_HANDLE_VALUE;

	texWidth = 640;		
	texHeight = 480;

	colorData = pColorData;
	depthData = pDepthData;

	initKinect();
}

void KinectReader::initKinect()
{
	// Count the Kinects connected to the PC
	HRESULT hr;
	int sensorCount = 0;
	hr = NuiGetSensorCount(&sensorCount);
	if (FAILED(hr)) throw runtime_error("No connected Kinects!");

	// Check all the Kinects connected to the PC
	for (int i = 0; i < sensorCount; i++) {
		hr = NuiCreateSensorByIndex(i, &kinect);
		if (FAILED(hr)) continue;

		// Check if we have a good connection
		hr = kinect->NuiStatus();
		if (hr == S_OK) break;	// No need to check further Kinects

		// Release the Kinect
		kinect->Release();
	}

	// Fail if no Kinect was good
	if (kinect == NULL) throw runtime_error("Could not connect to Kinect!");

	// Set the initialization flags for depth and color images
	hr = kinect->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
	if (!SUCCEEDED(hr)) throw runtime_error("Failed to open Kinect streams!");

	// Create data streams for depth and color
	kinect->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,
		NUI_IMAGE_RESOLUTION_640x480,
		0, 2, NULL, &colorStream);
	kinect->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH,
		NUI_IMAGE_RESOLUTION_640x480,
		0, 2, NULL, &depthStream);
}

GLubyte* KinectReader::kinectGetColor()
{
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT lockedRect;

	if (kinect->NuiImageStreamGetNextFrame(colorStream, 0, &imageFrame) < 0) return NULL;

	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &lockedRect, NULL, 0);

	if (lockedRect.Pitch != 0) {
		const BYTE* curr = (const BYTE*) lockedRect.pBits;
		const BYTE* dataEnd = curr + (texWidth * texHeight * 4);

		GLubyte* dest = colorData.data();
		while (curr < dataEnd) {
			*dest++ = *curr++;
		}
	}

	texture->UnlockRect(0);
	kinect->NuiImageStreamReleaseFrame(colorStream, &imageFrame);

	return colorData.data();
}

GLubyte* KinectReader::kinectGetDepth()
{
	HRESULT hr;
	NUI_IMAGE_FRAME imageFrame;

	// Attempt to get the depth frame
	hr = kinect->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame);
	if (FAILED(hr)) return NULL;

	BOOL nearMode;
	INuiFrameTexture* pTexture;

	// Get the depth image pixel texture
	hr = kinect->NuiImageFrameGetDepthImagePixelFrameTexture(depthStream, &imageFrame, &nearMode, &pTexture);
	if (FAILED(hr)) {
		// Release the frame
		kinect->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
		return NULL;
	}

	NUI_LOCKED_RECT lockedRect;

	// Lock the frame data
	pTexture->LockRect(0, &lockedRect, NULL, 0);

	// Make sure we're receiving valid data
	if (lockedRect.Pitch != 0) {
		// Get the min and max reliable depth for the current frame
		int minDepth = (nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
		int maxDepth = (nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

		const NUI_DEPTH_IMAGE_PIXEL* pBufferRun = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL*>(lockedRect.pBits);
		const NUI_DEPTH_IMAGE_PIXEL* pBufferEnd = pBufferRun + (texWidth * texHeight);

		int i = 0;
		while (pBufferRun < pBufferEnd) {
			USHORT depth = pBufferRun->depth;
			// Convert the depth from a short to a byte by discarding the 8 most significant bits.
			// This preserves precision but results in intensity "wrapping" when outside the range
			depthData[i++] = (GLubyte)(depth >= minDepth && depth <= maxDepth ? (depth - 32) % 256 : 0);
			pBufferRun++;
		}
	}

	// Unlock the rect
	pTexture->UnlockRect(0);
	pTexture->Release();

	// Release the frame
	kinect->NuiImageStreamReleaseFrame(depthStream, &imageFrame);

	return depthData.data();
}