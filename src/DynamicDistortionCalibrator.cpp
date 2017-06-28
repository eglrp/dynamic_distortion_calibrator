#include "DynamicDistortionCalibrator.h"

//_____________________________________________________________________________
DynamicDistortionCalibrator::DynamicDistortionCalibrator(int windowWidth , int windowHeight)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
}

//_____________________________________________________________________________
DynamicDistortionCalibrator::~DynamicDistortionCalibrator()
{
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::findRawDistortion()
{
	// find pixelSize
	//int pixelSize = findPixelSize();

	// find cameraArea
	//_border = findCameraBorder(pixelSize);
	//cout << "_borderArray[0][0] = " << _border._borderArray[0][0].x << "\n";
	// create images based on the found border
	createImages(2);

	// calculate the global offset from screen coordinates to image coordinates
	//calculateOffset();

	// correct for that offset, the values that remain in _area._distortionX/Y
	// are the local distortions
	// correctForOffset();
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::saveRawDistortion()
{
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::loadRawDistortion()
{
}

//_____________________________________________________________________________
int DynamicDistortionCalibrator::findPixelSize()
{
	// initialize pixelSize and pointer to pixelsize
	int pixelSize = 0;
	int *pixelSizePointer = &pixelSize;

	ofSetupOpenGL(_windowWidth, _windowHeight, OF_FULLSCREEN);// <-------- setup the GL context
	// create app for pixel size detection
	auto pixelSizeDetector = make_shared<PixelSizeDetector>();
	// set the app's pointer to the outside pixelSize variable
	pixelSizeDetector->setPixelSizePointer(pixelSizePointer);
	ofRunApp(pixelSizeDetector); // run app, closes once pixel found
	// print pixel size to screen
	std::cout << "DynDistCal: found pixelSize = " << pixelSize << "\n";

	return pixelSize;
}

//_____________________________________________________________________________
cameraBorder DynamicDistortionCalibrator::findCameraBorder(int pixelSize)
{
	// initialize the cameraBorder and pointer to it
	cameraBorder border;
	cameraBorder *areaPointer = &border;

	ofSetupOpenGL(_windowWidth, _windowHeight, OF_FULLSCREEN);// <-------- setup the GL context
	
	// create the app for camera area detection
	auto cameraBorderDetector = make_shared<CameraBorderDetector>();
	// set the app's pointer
	cameraBorderDetector->setCameraAreaPointerAndPixelSize(areaPointer, pixelSize);
	ofRunApp(cameraBorderDetector);
	std::cout << "found area \n";

	return border;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::createImages(int pixelSize)
{
	// initialize
	calibrationImage *verticalPointer = &_vertical;
	calibrationImage *horizontalPointer = &_horizontal;

	ofSetupOpenGL(_windowWidth, _windowHeight, OF_FULLSCREEN);// <-------- setup the GL context

	// create the app for the image creation
	auto imageCreator = make_shared<ImageCreator>();
	imageCreator->setImageReturnVariables(verticalPointer, horizontalPointer, _border, pixelSize);
	ofRunApp(imageCreator);
	std::cout << "_vertical.groundTruth[0][0].x = " << _vertical.groundTruth[0][0].x << "\n";
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::calculateOffset()
{

}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::correctForOffset()
{
	/*for (int x = 0; x < _area._sizeImageX; x++) {
		for (int y = 0; y < _area._sizeImageY; y++) {
			_area._distortionX[x][y] = _area._imageX[x][y] - _area._offsetX;
			_area._distortionY[x][y] = _area._imageY[x][y] - _area._offsetY;
		}
	}*/
}
