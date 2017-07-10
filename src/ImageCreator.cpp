#include "ImageCreator.h"


//_____________________________________________________________________________
void ImageCreator::setup() {
  // set framerate
  ofSetFrameRate(5);
  _cam.setVerbose(false);
  // check for external web cam and use that if possible
  if (_cam.listDevices().size() > 1) {
    _cam.setDeviceID(1);
  }
  else {
    _cam.setDeviceID(0);
  }

  // setup camera
  _cam.setup(_resolutionWidth, _resolutionHeight);

  _cam.update();

  // get screen dimensions
  _screenHeight = ofGetWindowHeight();
  _screenWidth = ofGetWindowWidth();

  // get image dimensions
  _img = _cam.getPixels();
  _imageHeight = _img.getHeight();
  _imageWidth = _img.getWidth();
  ofSetBackgroundAuto(false);
  ofBackground(ofColor::black);
  ofSetColor(ofColor::white);

  _state = 0;
  _drawCount = 0;
  _loopCount = 0;
}

//_____________________________________________________________________________
void ImageCreator::draw() {
	bool debug = true;

	if (_state == 0) {
		// entered drawing state
		_drawCount++;

		// draw vertical lines, if wanted
		if (_drawVertical == true) {
			for (int x = 0; x < _screenWidth;) {
				ofDrawRectangle(x, 0, _pixelSize * 5, _screenHeight);
				x += _pixelSize * 15;
			}
		}

		// draw horizontal lines, if wanted
		if (_drawHorizontal == true) {
			for (int y = 0; y < _screenHeight;) {
				ofDrawRectangle(0, y, _screenWidth, _pixelSize * 5);
				y += _pixelSize * 15;
			}
		}

		ofxCvGrayscaleImage img2 = _img;

		img2.resize(320, 240);
		img2.draw(0, 0);

		// next call capturing state
		if (_drawCount == 5) {  // minimum 4 for Nils' computer check all if you need higher timer!
			_drawCount = 0;
			_state = 1;
		}
	}
	else if (_state == 1) {
		// entered capturing state
		_cam.update();

		// get image from camera, conversion happens implicitly
		_color = _cam.getPixels();
		_img = _color;

		// next call calculation state
		_state = 0;
	}
	else {
		// something went wrong
		cout << "Something went wrong, unreachable state reached. Quitting now.\n";
		ofGetWindowPtr()->setWindowShouldClose();
	}
}

//_____________________________________________________________________________
void ImageCreator::mousePressed(int x, int y, int button) {
	std::cout << "mouse pressed\n";
	_cam.update();
	_color = _cam.getPixels();
	_img = _color;
	*_image = _img.getCvImage();

	ofImage img;
	img.allocate(_imageWidth, _imageHeight, ofImageType::OF_IMAGE_GRAYSCALE);
	img.setUseTexture(false);
	img.setFromPixels(_color.getPixels().getPixels(), _imageWidth, _imageHeight, OF_IMAGE_GRAYSCALE, false);
	/*std::cout << "pre-saving\n";
	img.save("distortedImage.bmp");// ("distortedImage.jpg", OF_IMAGE_QUALITY_BEST);
	//ofSaveImage(img.getPixels(), "distortedImage.jpg", OF_IMAGE_QUALITY_BEST);
	std::cout << "post-saving\n";*/
	ofGetWindowPtr()->setWindowShouldClose();
}

//_____________________________________________________________________________
void ImageCreator::setImagePointer(cv::Mat *&image)
{
	_image = image;
}

//_____________________________________________________________________________
void ImageCreator::setPixelSize(int pixelSize) {
	_pixelSize = pixelSize;
}

//_____________________________________________________________________________
void ImageCreator::setResolutionHeight(int resolutionHeight) {
	_resolutionHeight = resolutionHeight;
}

//_____________________________________________________________________________
void ImageCreator::setResolutionWidth(int resolutionWidth) {
	_resolutionWidth = resolutionWidth;
}

//_____________________________________________________________________________
void ImageCreator::setLinesToDraw(bool vert, bool hor) {
	_drawVertical = vert;
	_drawHorizontal = hor;
}

/*
void ImageCreator::findStraightBorderConnections() {
  // find horizontal lines
  for (int y = 0; y < _imageHeight; y += 2*_pixelSize) {
    int lowerX = 0;
    int count = 0;
    line actualLine;
    for (int x = 0; x < _imageWidth; x++) {
      if (_border._borderArray[x][y].border) {
        actualLine.y1 = actualLine.y2 = y;
        if (lowerX == 0) {
          lowerX = x;
          count++;
        }
        if (lowerX < x) {
          actualLine.x1 = lowerX;
          actualLine.x2 = x;
          count++;
        }
        if ((count == 2) && (actualLine.x1 != actualLine.x2) && (actualLine.x1 != 0) && (actualLine.x2 != 0)) {
          actualLine.horizontal = true;
          _horizontals.push_back(actualLine);
          count = 0;
          lowerX = 0;
        }
      }
    }
  }
  // find vertical lines
  for (int x = 0; x < _imageWidth; x += 2 * _pixelSize) {
    int lowerY = 0;
    line actualLine;
    int count = 0;
    for (int y = 0; y < _imageHeight; y++) {
      if (_border._borderArray[x][y].border) {
        actualLine.x1 = actualLine.x2 = x;
        if (lowerY == 0) {
          lowerY = y;
          count++;
        }
        if (lowerY <y) {
          actualLine.y1 = lowerY;
          actualLine.y2 = y;
          count++;
        }
        if ((actualLine.y1 != actualLine.y2) && (actualLine.y1 != 0) && (actualLine.y2 != 0)) {
          actualLine.vertical = true;
          _verticals.push_back(actualLine);
          count = 0;
          lowerY = 0;
        }
      }
    }
  }
  saveGroundTruth(_horizontals);
  saveGroundTruth(_verticals);
}*/

/*
void ImageCreator::drawDebug()
{
  ofImage img;
  img = _diffPixels;// difference between _cam;
  img.draw(0, 0);
  _cam.draw(0, 240);
  ofImage background;
  background = _background;
  background.draw(0, 480);
}
*/

/*
void ImageCreator::saveGroundTruth(vector<line> &vectorOfLines) {
  // go threw the vectorOfLines and set borders of the 2 for-loops to
  // x1 -> x2 and y1 -> y2
  // so every pixel on the line should be drawn
  for (auto it : vectorOfLines){
    for (int y = it.y1; y <= it.y2; y++) {
      for (int x = it.x1; x <= it.x2; x++) {
        if (it.horizontal && !it.vertical)
        {
          _horizontal->groundTruth[x][y].x = x;
          _horizontal->groundTruth[x][y].y = y;
          _horizontal->groundTruth[x][y].b = 255;
        }
        // test isn't necessary but possible
        else if(it.vertical && !it.horizontal) {
          _vertical->groundTruth[x][y].x = x;
          _vertical->groundTruth[x][y].y = y;
          _vertical->groundTruth[x][y].b = 255;
        }
      }
    }
  }
}
*/
/*
void ImageCreator::countLines(cv::Mat &distImage) {
  // initial values
  cv::Mat vert = _vertical->_image;
  int* offsetX = new int[_imageHeight];
  offsetX = _vertical->_verticalOffsets;
  int* offsetY = new int[_imageWidth];
  offsetY = _horizontal->_horizontalOffsets;
  pos initialPos = _vertical->_initialPosition;

  // return values
  int* linesVert = new int[_imageHeight];
  int* linesHorizon = new int[_imageWidth];


  for (size_t y = initialPos.y; y < _imageHeight; y++){
    size_t initX = offsetX[y];
    int countLines = 0;
    int lastVal = -1;
    for (size_t x = initX; x < _imageWidth; x++) {
      size_t val = vert.at<uchar>(x, y);
      if (val == 255 && lastVal != val) {
        countLines++;
      }
      else if(val == 0 && lastVal != val) {
        countLines++;
      }
      lastVal = val;
    }
    linesVert[y] = countLines;
  }
  for (size_t y = initialPos.y; y < 0; y--) {
    size_t initX = offsetX[y];
    int countLines = 0;
    int lastVal = -1;
    for (size_t x = initX; x < 0; x--) {
      size_t val = vert.at<uchar>(x, y);
      if (val == 255 && lastVal != val) {
        countLines++;
      }
      else if (val == 0 && lastVal != val) {
        countLines++;
      }
      lastVal = val;
    }
    linesVert[y] = countLines;
  }



  for (size_t x = initialPos.x; x < _imageWidth; x++) {
    size_t initY = offsetY[x];
    int countLines = 0;
    int lastVal = -1;
    for (size_t y = initY; y < _imageHeight; y++) {
      size_t val = vert.at<uchar>(x, y);
      if (val == 255 && lastVal != val) {
        countLines++;
      }
      else if (val == 0 && lastVal != val) {
        countLines++;
      }
      lastVal = val;
    }
    linesHorizon[x] = countLines;
  }

  for (size_t x = initialPos.x; x < 0; x--) {
    size_t initY = offsetY[x];
    int countLines = 0;
    int lastVal = -1;
    for (size_t y = initY; y < 0; y--) {
      size_t val = vert.at<uchar>(x, y);
      if (val == 255 && lastVal != val) {
        countLines++;
      }
      else if (val == 0 && lastVal != val) {
        countLines++;
      }
      lastVal = val;
    }
    linesHorizon[x] = countLines;
  }



}
*/



void ImageCreator::interpolateLines(cv::Mat &matchMat , bool vert) {
  // should be set in function use
  vert = true;
  if (vert) {
    for (size_t x = 0; x < _imageWidth; x++)
    {
      int emptyCellCount = 0;
      int lastVal = 0;
      int actualVal = 0;
      for (size_t y = 0; y < _imageHeight; y++)
      {
        // checks if the actual seen pos got a value and save that to actualVal and the last actualVal to lastVal
        if ((matchMat.at<uchar>(x, y) != -1) && (actualVal == 0))
        {
          lastVal = actualVal;
          actualVal = matchMat.at<uchar>(x, y);
        }
        // every time the emptyCellCount > 0 and a cell is filled the cells between the last filled and actual were interpolated linear
        if ((emptyCellCount > 0) && (actualVal != 0)) {
          for (size_t yy = 0; yy < emptyCellCount; yy++)
          {
            matchMat.at<uchar>(x, y - emptyCellCount + yy) = ((actualVal - lastVal) / emptyCellCount)*yy;
          }
          actualVal = 0;
        }

        if ((matchMat.at<uchar>(x, y) == -1))
        {
          emptyCellCount++;
        }
        else {
          emptyCellCount = 0;
        }
      }
    }
  }
  else {
    for (size_t y = 0; y < _imageHeight; y++)
    {
      int emptyCellCount = 0;
      int lastVal = 0;
      int actualVal = 0;
      for (size_t x = 0; x < _imageWidth; x++)
      {
        if ((matchMat.at<uchar>(x, y) != -1) && (actualVal == 0))
        {
          lastVal = actualVal;
          actualVal = matchMat.at<uchar>(x, y);
        }
        if ((emptyCellCount > 0) && (actualVal != 0) ) {
          for (size_t xx = 0; xx < emptyCellCount; xx++)
          {
            matchMat.at<uchar>(x - emptyCellCount + xx, y ) = ((actualVal - lastVal) / emptyCellCount)*(xx+1);
          }
          actualVal = 0;
        }
        if ((matchMat.at<uchar>(x, y) == -1))
        {
          emptyCellCount++;
        }
        else {
          emptyCellCount = 0;
        }
      }
    }
  }
}