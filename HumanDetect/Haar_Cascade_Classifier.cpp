#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

//CASCADE CLASSIFIER
#include "opencv2/objdetect/objdetect.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

class HaarBodyClassifier {
	CascadeClassifier classifier;
public: 
	bool status;
	double scaleFactor;
	int minNeighbours;
	int flags;
	Size minSize;

	///Accepts a XML name of a file describing a classifier
	HaarBodyClassifier(string cascadeName) {
		scaleFactor = 1.1;
		minNeighbours = 2;
		flags = 0 | CV_HAAR_SCALE_IMAGE;
		minSize = Size(15, 25);

		status = classifier.load(cascadeName);
		if (!status)
		{
			cerr << "Error creating a classifier from a file: '" << cascadeName << "'" << endl;
		}
	}

	///Works with a grayscale Mat
	std::vector<Rect> detect(Mat& frame)
	{
		std::vector<Rect> detected;
		classifier.detectMultiScale(frame, detected, 1.1, 2, flags, minSize);
		return detected;
	}

	///Turns an image into a grayscale and applies equalizeHist
	std::vector<Rect> detectAuto(Mat& frame)
	{
		std::vector<Rect> detected;
		Mat frame_gray;
		cvtColor(frame, frame_gray, CV_BGR2GRAY);
		// equalize
		equalizeHist(frame_gray, frame_gray);

		// Detect 
		classifier.detectMultiScale(frame_gray, detected,  1.1, 2, flags, minSize);
		return detected;
	}
	
	~HaarBodyClassifier() {

	}
};
