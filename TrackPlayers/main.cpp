#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"
#include "HOGDetector.hpp"
#include "Detector.hpp"
#include "tools.hpp"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;
using namespace cv::ml;


//draws detected (rectangles) over the image
void DrawDetected(Mat& frame, std::vector<Rect>& detected, std::vector<double>& weights, Scalar color = Scalar(0, 0, 255), int thinkness = 1);
//saves an image to a file
bool save(Mat& frame, string toFile);
void MarkDetectedPlayer(Mat& img, Rect& detectedRect, double weight);

int main(int argc, char** argv) {
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	if (argc < 2) {
		cerr << "No input image " << endl;
		return 1;
	}
	
	/* 
	//This piece of cide if for an unput type video
	if (false){
		cv::VideoCapture cap = cv::VideoCapture(argv[1]);

		if (!cap.isOpened()) { // check if we succeeded
			std::cerr << "Couldn't open video capture." << std::endl;
			return -1;
		}

		double fps = cap.get(CV_CAP_PROP_FPS);
		// Default resolution of the frame is obtained.The default resolution is system dependent.
		int inputVideoFrameWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int inputVideoFrameHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		//get total frame count
		int totalFrames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT);

		int brighter = 10;
		int frameNumber = 1;

		cv::Mat videoFrame;
		for (;;) {

			cap >> videoFrame;
			frameNumber++;
			if (frameNumber > totalFrames) {
				//break;
				frameNumber = 1;
				cap.set(CV_CAP_PROP_POS_FRAMES, 0); // reset video frame to 0
			}
			char option = waitKey(fps);
			if (option == 13)
				break;
		}
	}
	*/

	string imageName = argv[1];
	Mat img = cv::imread(imageName);
	if (img.empty()) {
		cerr << "could not open image named '" << argv[1] << ",\n";
	}

	//File Name
	std::size_t dotIdx = imageName.find_last_of(".");
	string outFileNameWithoutExt = imageName.substr(0, dotIdx);
	size_t slashIdx = imageName.find_last_of("/");
	string outFileName = outFileNameWithoutExt.substr(slashIdx + 1);
	string outFileExt = imageName.substr(dotIdx);

	if (true) {

		Detector d;
		d.DetectWithPreprocessing(img);
		
		cv::Mat out = img.clone();
		DrawDetected(out, d.detected, d.weights);

		string outfileName = outFileName + "-out-final-Result" + outFileExt;
		save(out, outfileName);

		cv::Mat outMarked = img.clone();
		for (size_t i = 0; i < d.detected.size(); i++)
		{
			if (d.weights[i] > 0.1)
				MarkDetectedPlayer(outMarked, d.detected[i], d.weights[i]);
		}
		save(outMarked, outFileName + "-out-final-Marked" + outFileExt);
		imshow("Result", outMarked);
	}

	waitKey(0);
	return 0;
}

//METHODS
//Draw detected rectangles on the image
void DrawDetected(Mat& frame, std::vector<Rect>& detected, std::vector<double>& weights, Scalar color, int thinkness) {
	if (detected.size() > 0) {
		for (int gg = 0; gg < detected.size(); gg++) {
			rectangle(frame, detected[gg].tl(), detected[gg].br(), color, thinkness, 8);
			putText(frame, to_string(weights[gg]), Point(detected[gg].x, detected[gg].y), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 20, 20), 2);
		}
	}
}

//save image
bool save(Mat& frame, string toFile) {
	return cv::imwrite(toFile, frame);
}

/*
	Mark players on rink using detected rectangle 
*/
void MarkDetectedPlayer(Mat& img, Rect& detectedRect, double weight) {
	Rect r = rectForCircle(detectedRect);
	//cv::rectangle(img, r, Scalar(255, 0, 0), 1);
	drawCircle(img, r);
	

}
