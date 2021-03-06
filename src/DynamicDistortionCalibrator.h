#pragma once
#include "cameraArea.h"
#include "PixelSizeDetector.h"
#include "CameraAreaDetector.h"
#include "ImageCreator.h"
#include "Constants.h"


class DynamicDistortionCalibrator {
	public:
		// constructor
		DynamicDistortionCalibrator(int windowWidth , int windowHeight);
		// destructor
		~DynamicDistortionCalibrator();

		// stuff for raw distortion aka local offset in x & y direction
		void findRawDistortion(int** &matchX, int** &matchY);
		void saveRawDistortion(string path);
		void loadRawDistortion(string path);
		// undistorts the given image, using the given distortion maps
		ofImage undistort(cv::Mat distortedImage, int** matchX, int** matchY);
		
		// creates an image for distortion testing purposes
		// vert: draw vertical lines
		// hor: draw horizontal lines
		// save: save content of screen in a .jpg
		ofxCvGrayscaleImage createImage(bool vert = true, bool hor = true);

		// setter for the spacing in the cameraAreaDetector
		void setSpacing(int spacing);
		// getter for the spacing in the cameraAreaDetector
		int getSpacing();

		// setter for camera resolution's width
		void setResolutionWidth(int resolutionWidth);
		// getter for camera resolution's width
		int getResolutionWidth();

		// setter for camera resolution's height
		void setResolutionHeight(int resolutionHeight);
		// getter for camera resolution's height
		int getResolutionHeight();

		// setter for lower threshold for canny edge detection
		void setCannyLowerThreshold(int lowerThreshold);
		// getter for lower threshold for canny edge detection
		int getCannyLowerThreshold();

		// setter for upper threshold for canny edge detection
		void setCannyUpperThreshold(int upperThreshold);
		// getter for upper threshold for canny edge detection
		int getCannyUpperThreshold();

		// setter for jump width
		void setJump(int jump);
		// getter for jump width
		int getJump();

		// getter for maps
		int** getMapX();
		int** getMapY();
		// setter for maps
		void setMaps(int** mapX, int** mapY);

		// interpolate missing allocations in a vertical or horizontal line in an image
		int ** interpolateLines(int ** matchMat, bool vert);

		// calculates the part of the screen that was seen by the camera and thus it's content
		ofImage createGroundTruthFromImageAndMap(ofImage img, int** mapX, int** mapY);
		
		// gives a comparison of the ground truth and the undistorted image
		void compareResults(ofImage gt, ofImage resImg, ofImage *&difference, int *&noDiff, float *&ratioDiff);

	private:
		// contains information about the visible camera area
		cameraArea _area;
		// height and width of the screen
		int _windowHeight, _windowWidth;
		calibrationImage _vertical, _horizontal;
		// spacing for center point estimation in camera area detection
		int _spacing;
		// resolution of the camera
		int _resolutionWidth, _resolutionHeight;
		// thresholds for canny detector
		int _cannyLower, _cannyUpper;
		// minimally visible pixelSize
		int _pixelSize;
		// width by which the rectangle in the camera area detection shrink
		int _jump;
		// bool array that contains whether a specific pixel needs to be interpolated
		bool** _interpolate;

		// finds teh minimally visible pixel size
		int findPixelSize();
		// finds the visible camera area
		cameraArea findCameraArea();

		// returns the undistorted image with interpolated pixels in the inner of the frame
		cv::Mat interpolateImage(cv::Mat undistedImage);
		// returns the undistorted image
		cv::Mat mappingImage(cv::Mat distordedImage, int** matchY, int** matchX);
		// breaks up a line into an array
		int* stringToIntArray(string line);
		
};