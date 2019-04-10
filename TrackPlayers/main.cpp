#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"
#include "HOGDetector.cpp"
#include "tools.hpp"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

//draws detected (rectangles) over the image
void DrawDetected(Mat& frame, std::vector<Rect>& detected, Scalar color = Scalar(0, 0, 255), int thinkness = 1);
//saves an image to a file
bool save(Mat& frame, string toFile);
double PrepareForProcessing(Mat& in, Mat& out, int colorSpace = COLOR_BGR2YUV, double contrast = 0);
void MakeMaskFromEges(Mat& src, Mat& dst);
void MakeMask(Mat& src, Mat& dst);

void MarkDetectedPlayer(Mat& img, Rect& detectedRect);

//TESTS
int main(int argc, char** argv) {
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	if (argc < 2) {
		cerr << "No input image " << endl;
		return 1;
	}
	/*
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
		vector<Rect> detected;
		vector<double> weights;
		double hitThreshold = 0;
		Size winStride = Size(4, 4);
		Size padding = Size(4, 4);
		double scale = 1.05;
		Mat preparedImage;
		for (;;) {

			cap >> videoFrame;
			cv::resize(videoFrame, videoFrame, Size(), 0.6, 0.6);

			PrepareForProcessing(videoFrame, preparedImage);
			hog.detectMultiScale(preparedImage, detected, weights, hitThreshold, winStride, padding, scale, 2, false);

			DrawDetected(videoFrame, detected);

			for (auto d : detected)
			{
				MarkDetectedPlayer(videoFrame, d);
			}
			imshow("Result", videoFrame);
			imshow("Process", preparedImage);

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

		double hitThreshold = 0;
		// Window stride.It must be a multiple of block stride
		Size winStride = Size(4, 4);
		Size padding = Size(16, 16);
		//oefficient of the detection window increase.
		//scaleFactor - Parameter specifying how much the image size is reduced at each image scale.
		double scale = 1.05;
		//group_threshold – Coefficient to regulate the similarity threshold. When detected, some objects can be covered by many rectangles. 0 means not to perform grouping. See groupRectangles() .
		double group_threshold = 2;
		vector<Rect> detected;
		vector<double> weights;

		Mat mask, preparedImage, preparedImageMasked;
		MakeMask(img, mask);
		PrepareForProcessing(img, preparedImage, COLOR_RGB2YUV, -10);
		applyMask(preparedImage, preparedImageMasked, mask);
		imwrite("Prepared.jpg", preparedImageMasked);

		hog.detectMultiScale(preparedImageMasked, detected, weights, hitThreshold, winStride, padding, scale, group_threshold, false);

		cv::Mat out = img.clone();
		DrawDetected(out, detected);

		string outfileName = outFileName + "-out-Result" + outFileExt;
		save(out, outfileName);

		cv::Mat outMarked = out.clone();
		for (auto d : detected)
		{
			MarkDetectedPlayer(outMarked, d);
		}
		save(outMarked, "marked.jpg");
		imshow("Result", outMarked);
	}
	
	//waitKey(0);
	return 0;
}

//METHODS
void DrawDetected(Mat& frame, std::vector<Rect>& detected, Scalar color, int thinkness) {
	if (detected.size() > 0) {
		for (int gg = 0; gg < detected.size(); gg++) {
			rectangle(frame, detected[gg].tl(), detected[gg].br(), color, thinkness, 8);
		}
	}
}

bool save(Mat& frame, string toFile) {
	return cv::imwrite(toFile, frame);
}

double PrepareForProcessing(Mat& src, Mat& dst, int colorSpace, double contrast) {

	//cv::cvtColor(in, out, colorSpace);

	addContrast(src, dst, contrast);

	Mat imageHSV, diff;
	cv::cvtColor(src, imageHSV, COLOR_RGB2HSV);

	absdiff(src, imageHSV, diff);
	absdiff(imageHSV, diff, dst);

	//out *= 1.2;
	//out += cv::Scalar(brighter, brighter, brighter); //make the copy brighter

	//equalizeHist(in, out);
	/*
	//This does not help much

	double sp =10.0, sr=20.0;
	int maxLevel = 1;
	cv::pyrMeanShiftFiltering(out, out, sp, sr, maxLevel);
	//string fileName = "MeanShift_sp" + to_string(sp) + "-sr" + to_string(sr) + "-max" + to_string(maxLevel) + ".jpg";
	*/
	/*
	cv::Mat mask;
	MakeMask(in, mask);

	in.copyTo(out, mask);
	cv::cvtColor(out, out, CV_RGB2GRAY);
	*/
	return 1;
	/*
	//scale
	double maxWidth = 1000;
	int width = in.size().width;
	double scale = maxWidth / (double)width;
	cv::resize(out, out, Size(), scale, scale);

	createBinaryImage(out, out, 200, 255);
	return scale;
	*/
}

void MakeMaskFromEges(Mat& src, Mat& dst) {
	int cannyThreshold1 = 230;
	int cannyThreshold2 = 250;

	cv::cvtColor(src, dst, CV_RGB2GRAY);

	Mat canny;
	Canny(dst, canny, cannyThreshold1, cannyThreshold2, 3);
	imshow("Canny", canny);

	// 4) Dilate to remove noise
	cv::Mat cannyMaskedDilated;  // Result output
	cv::dilate(canny, cannyMaskedDilated, Mat(), Point(-1, -1), 1, 1, 1);
	imshow("Canny_Dilated", cannyMaskedDilated);

	//	Apply morphological closing to the binary image using a 3x3 square kernel
	int kernelSize = 3; //kernel size
	cv::Mat elem = 255 * cv::Mat::ones(kernelSize, kernelSize, CV_8UC1); // 3x3 kernel matrix filled with 255
	cv::Mat closed;
	cv::morphologyEx(cannyMaskedDilated, closed, MORPH_CLOSE, elem);
	cv::imshow("Closed", closed);
	dst = closed.clone();
}

void MakeMask(Mat& src, Mat& dst) {

	/*
	Algorithm:
	1) Add contrast
	2) Convert to grayscale
	3) Threshhold gray image
		Use big kernel size to remove players and noise from the rink
	4) Apply morph opening to remove small details
	5) Blur the image
		Use high value, like 25
	6) Dilate to remove some big chunks
	7) Erode to return to get back lost area of the rink
	9) Use bitwise flip to get a mask

	*/
	// 1) add contrast
	Mat img;
	addContrast(src, img, 30);

	//2) MAKE GRAY
	Mat gray, binary, binaryBlured;
	cv::cvtColor(img, gray, CV_RGB2GRAY);

	//3) BINARY FROM GRAY
	int binaryImageThreshold = 100;
	createBinaryImage(gray, binary, binaryImageThreshold, 255);
	//imshow("Binary", binary);

	// 4) Open thresholded image
	int kernelSize = 25; //kernel size
	cv::Mat elem = 255 * cv::Mat::ones(kernelSize, kernelSize, CV_8UC1); // 19x19 kernel matrix filled with 255
	cv::morphologyEx(binary, binary, MORPH_OPEN, elem);

	// 5) BLUR BINARY
	cv::medianBlur(binary, binary, 25);
	//imshow("Blur Binary", binaryBlured);

	// 6) Open blured
	cv::dilate(binary, binary, elem);
	cv::dilate(binary, binary, elem);

	// 6) Erode
	int erodekernelSize = 15; //kernel size
	cv::Mat erodeelem = 255 * cv::Mat::ones(erodekernelSize, erodekernelSize, CV_8UC1); // 19x19 kernel matrix filled with 255
	cv::erode(binary, binary, erodeelem);
	cv::bitwise_not(binary, dst);
}

void MarkDetectedPlayer(Mat& img, Rect& detectedRect) {
	Rect r = rectForCircle(detectedRect);
	cv::rectangle(img, r, Scalar(255, 0, 0), 1);
	drawCircle(img, r);
}
