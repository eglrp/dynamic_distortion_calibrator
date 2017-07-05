#include "DynamicDistortionCalibrator.h"

//_____________________________________________________________________________
DynamicDistortionCalibrator::DynamicDistortionCalibrator(int windowWidth , int windowHeight)
{
	_pixelSize = 1;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
}

//_____________________________________________________________________________
DynamicDistortionCalibrator::~DynamicDistortionCalibrator()
{
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::findRawDistortion(int** &matchX, int** &matchY)
{
	// find pixelSize
	_pixelSize = findPixelSize();

	// find cameraArea
	_area = findCameraArea();

	matchX = _area._distortionX;
	matchY = _area._distortionY;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::saveRawDistortion(string path)
{
	ofstream file(path);

	if (file.is_open()) {
		// first print the sizes of the arrays
		file << _area._sizeImageX << "\n";
		file << _area._sizeImageY << "\n";

		// then print the arrays, first x-distortion
		for (int x = 0; x < _area._sizeImageX; x++) {
			for (int y = 0; y < _area._sizeImageY; y++) {
				file << _area._distortionX[x][y];
				if (y != _area._sizeImageY - 1) {
					file << ", ";
				}
			}
			file << ";\n";
		}
		// leave a line out
		std::cout << "saved x-map\n";
		file << "\n";
		// then y-distortion
		for (int x = 0; x < _area._sizeImageX; x++) {
			for (int y = 0; y < _area._sizeImageY; y++) {
				file << _area._distortionX[x][y];
				if (y != _area._sizeImageY - 1) {
					file << ", ";
				}
			}
			file << "\n";
		}
		std::cout << "saved y-map\n";
	}
	else {
		std::cout << "Saving the maps went wrong. File could not be openend.\n";
	}
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::loadRawDistortion(string path)
{
	ifstream file(path);
	string line;

	if (file.is_open()) {
		// first line is width of the array
		getline(file, line);
		int width = stoi(line);
		// second line is height of the array
		getline(file, line);
		int height = stoi(line);

		// allocate the arrays
		_area._sizeImageX = width;
		_area._sizeImageY = height;

		std::cout << "loaded sizes: " << width << " x " << height << "\n";

		_area._distortionX = new int*[width];
		_area._distortionY = new int*[width];

		for (int i = 0; i < width; i++) {
			_area._distortionX[i] = new int[height];
			_area._distortionY[i] = new int[height];
		}

		int count = 0;
		bool xComplete = false;
		// load x map
		// get first line
		getline(file, line);
		while(xComplete == false) {
			// work on that line
			_area._distortionX[count] = stringToArray(line);
			// get next line
			getline(file, line);
			++count;
			// if it's only a line break, cease
			if (line.compare("\n") == 0 || line.compare("") == 0) {
				xComplete = true;
			}
		}
		std::cout << "loaded x-map\n";
		// load y map
		count = 0;
		getline(file, line);
		while (!line.empty()) {
			// work on that line
			_area._distortionY[count] = stringToArray(line);
			// get next line
			getline(file, line);
			++count;
		}
		
	}
	else {
		std::cout << "Reading the maps went wrong. File could not be opened.\n";
	}
}

//_____________________________________________________________________________
int* DynamicDistortionCalibrator::stringToArray(string line) {
	string tmp;
	vector<int> vec;
	while (line.compare(";") != 0 && line.compare("") != 0) {
		tmp = line.substr(0, line.find_first_of(","));
		vec.push_back(stoi(tmp));
		line = line.substr(line.find_first_of(",") + 2, line.size());
	}

	int* arr = new int[vec.size()];
	for (size_t i = 0; i < vec.size(); i++) {
		arr[i] = vec.at(i);
	}

	return arr;
}

//_____________________________________________________________________________
ofxCvGrayscaleImage DynamicDistortionCalibrator::createImage(bool vert, bool hor) {
	ofImage tmp;
	ofxCvGrayscaleImage returnImage;
	cv::Mat image;
	cv::Mat* imagePointer = &image;
	
	if (_pixelSize == -1) {
		std::cout << "ERROR: _pixelSize unknown. Returned image is unallocated!\n";
		return returnImage;
	}

	ofSetupOpenGL(_windowWidth, _windowHeight, OF_FULLSCREEN);// <-------- setup the GL context

	auto imageCreator = make_shared<ImageCreator>();

	imageCreator->setImagePointer(imagePointer);
	imageCreator->setPixelSize(_pixelSize);
	imageCreator->setResolutionHeight(_resolutionHeight);
	imageCreator->setResolutionWidth(_resolutionWidth);
	imageCreator->setLinesToDraw(vert, hor);

	ofRunApp(imageCreator);

	tmp.setFromPixels((unsigned char*)IplImage(image).imageData, image.size().width,
		image.size().height, OF_IMAGE_GRAYSCALE);
	returnImage = tmp;

	return returnImage;
}

//_____________________________________________________________________________
ofImage DynamicDistortionCalibrator::undistort(cv::Mat distortedImage, int** matchX, int** matchY) {
	cv::Mat undistorted = mappingImage(distortedImage, matchX, matchY);
	cv::Mat interpolated = interpolateImage(undistorted);
	// conversion to ofImage
	ofImage img;
	img.setFromPixels((unsigned char*)IplImage(interpolated).imageData, interpolated.size().width,
		interpolated.size().height, OF_IMAGE_GRAYSCALE);
	return img;
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
	pixelSizeDetector->setResolutionHeight(_resolutionHeight);
	pixelSizeDetector->setResolutionWidth(_resolutionWidth);
	ofRunApp(pixelSizeDetector); // run app, closes once pixel found

	return pixelSize;
}

//_____________________________________________________________________________
cameraArea DynamicDistortionCalibrator::findCameraArea()
{
	// initialize the cameraArea and pointer to it
	cameraArea area;
	cameraArea *areaPointer = &area;

	ofSetupOpenGL(_windowWidth, _windowHeight, OF_FULLSCREEN);// <-------- setup the GL context
	
	// create the app for camera area detection
	auto cameraAreaDetector = make_shared<CameraAreaDetector>();
	// set the app's pointer
	cameraAreaDetector->setCameraAreaPointerAndPixelSize(areaPointer, _pixelSize);
	// set the spacing, was calculated for our setup as optimal with a value of 34
	cameraAreaDetector->setSpacing(_spacing);
	cameraAreaDetector->setResolutionHeight(_resolutionHeight);
	cameraAreaDetector->setResolutionWidth(_resolutionWidth);
	cameraAreaDetector->setCannyLowerThreshold(_cannyLower);
	cameraAreaDetector->setCannyUpperThreshold(_cannyUpper);
	ofRunApp(cameraAreaDetector);
	std::cout << "found area \n";
	std::cout << area._distortionX[0][0] << "\n";

	return area;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::setSpacing(int spacing) {
	_spacing = spacing;
}

//_____________________________________________________________________________
int DynamicDistortionCalibrator::getSpacing() {
	return _spacing;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::setResolutionWidth(int resolutionWidth) {
	_resolutionWidth = resolutionWidth;
}

//_____________________________________________________________________________
int DynamicDistortionCalibrator::getResolutionWidth() {
	return _resolutionWidth;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::setResolutionHeight(int resolutionHeight) {
	_resolutionHeight = resolutionHeight;
}

//_____________________________________________________________________________
int DynamicDistortionCalibrator::getResolutionHeight() {
	return _resolutionHeight;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::setCannyUpperThreshold(int upperThreshold) {
	_cannyUpper = upperThreshold;
}

//_____________________________________________________________________________
int DynamicDistortionCalibrator::getCannyUpperThreshold() {
	return _cannyUpper;
}

//_____________________________________________________________________________
void DynamicDistortionCalibrator::setCannyLowerThreshold(int lowerThreshold) {
	_cannyLower = lowerThreshold;
}

//_____________________________________________________________________________
int DynamicDistortionCalibrator::getCannyLowerThreshold() {
	return _cannyLower;
}

//_____________________________________________________________________________
cv::Mat DynamicDistortionCalibrator::interpolateImage(cv::Mat undistedImage) {
	cv::Mat interpolatedImage;
	// allocate the interpolatedImage that will be the return of the function with the size of the 
	// camera image
	interpolatedImage.zeros(undistedImage.size(), undistedImage.type());
	int rows = undistedImage.rows;
	int cols = undistedImage.cols;

	for (size_t x = 0; x < rows; x++) {
		for (size_t y = 0; y < cols; y++) {
			// if a seen value is at (x,y) position use that
			if (undistedImage.at<uchar>(x, y) != -1) {
				interpolatedImage.at<uchar>(x, y) = undistedImage.at<uchar>(x, y);
			}
			// else use bilinear interpolation of all accessible pixels around (x,y)
			else {
				size_t interpolate = 0;
				size_t count = 0;
				if (x + 1 < rows) {
					interpolate += (size_t)undistedImage.at<uchar>(x + 1, y);
					count++;
				}
				if (y + 1 < cols)
				{
					interpolate += (size_t)undistedImage.at<uchar>(x, y + 1);
					count++;
				}
				if (x - 1 > 0) {
					interpolate += (size_t)undistedImage.at<uchar>(x - 1, y);
					count++;
				}
				if (y - 1 < 0)
				{
					interpolate += (size_t)undistedImage.at<uchar>(x, y - 1);
					count++;
				}
				// if less than 2 pixel next to the not seen pixel are seen then it has to be at the border or not detected by the camera
				if (count > 1) {
					interpolatedImage.at<uchar>(x, y) = (uchar)floor(interpolate / count);
				}
			}
		}
	}
	return interpolatedImage;
}

//_____________________________________________________________________________
cv::Mat DynamicDistortionCalibrator::mappingImage(cv::Mat distortedImage, int** matchX, int** matchY) {
	cv::Mat mappedImage;
	size_t rows = distortedImage.rows;
	size_t cols = distortedImage.cols;
	size_t maxX, maxY, minX, minY;
	maxX = 0;
	maxY = 0;
	minX = rows;
	minY = cols;

	// find the extremal values of x & y  and save them for correct allocation of the mappedImage
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			size_t x = (size_t)matchX[i][j];
			size_t y = (size_t)matchY[i][j];
			if (x > maxX)
			{
				maxX = x;
			}
			if (y > maxY)
			{
				maxY = y;
			}
			if (x < minX)
			{
				minX = x;
			}
			if (y < minY)
			{
				minY = y;
			}
		}
	}
	// allocates the mapped undistorted matrix with zeros
	mappedImage.zeros(maxX - minX, maxY - minY, distortedImage.type());

	// checks the found matching matrices for the (x,y) distortion values and 
	// move the color values to the correct position 
	for (size_t x = 0; x < rows; x++) {
		for (size_t y = 0; y < cols; y++) {
			if (matchX[x][y] != -1 && matchY[x][y] != -1) {
				size_t distX = (size_t)matchX[x][y] - minX + 1;
				size_t distY = (size_t)matchY[x][y] - minY + 1;
				if (distX >= 0 && distY >= 0)
				{
					mappedImage.at<uchar>(distX, distY) = distortedImage.at<uchar>(x, y);
				}
			}
		}
	}
	return mappedImage;
}