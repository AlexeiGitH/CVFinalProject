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
double PrepareForProcessing(Mat& in, Mat& out, int colorSpace = COLOR_BGR2YUV);
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
			Size winStride = Size(10, 10);
			Size padding = Size(16, 16);
			double scale = 1.05;

			vector<Rect> detected;
			vector<double> weights;
			Mat preparedImage;
			PrepareForProcessing(img, preparedImage);

			imwrite("Prepared.jpg", preparedImage);

			//waitKey(0);
			hog.detectMultiScale(preparedImage, detected, weights, hitThreshold, winStride, padding, scale, 2, false);

			cv::Mat out = img.clone();
			DrawDetected(out, detected);

			string outfileName = outFileName + "-out-unchanged-HOG-yuv"  + outFileExt;
			save(out, outfileName);

			cv::Mat outMarked = out.clone();
			for (auto d : detected)
			{
				MarkDetectedPlayer(outMarked, d);
			}
			save(outMarked, "marked.jpg");
			imshow("Result", outMarked);
		}
		if (false) {
			Mat mask, prepared;
			MakeMask(img, mask);
			img.copyTo(prepared, mask);

			//Mat prepared;
			//double scaleResized = PrepareForProcessing(img, prepared);

			imshow("preparedImage", prepared);

			vector<Rect> detected;
			vector<double> weights;
			double hitThreshold = 0;
			Size winStride = Size(8, 8);
			Size padding = Size(32, 32);
			double scale = 1.05;
			hog.detectMultiScale(prepared, detected, weights, hitThreshold, winStride, padding, scale, 2, false);
			/*
			for (size_t i = 0; i < detected.size(); i++)
			{
				Rect scaledRect = scaleRect(detected[i], 1/scaleResized);
				detected[i] = scaledRect;
			}*/
			cv::Mat out = img.clone();
			DrawDetected(out, detected);

			string outfileName = outFileName + "-out-prepared-HOG" + outFileExt;
			save(out, outfileName);
		}
	
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

double PrepareForProcessing(Mat& in, Mat& out, int colorSpace ) {
	//make gray
	cv::cvtColor(in, out, colorSpace);


	//out *= 1.2;
	//out += cv::Scalar(brighter, brighter, brighter); //make the copy brighter

	//equalizeHist(in, out);
	/*
	This does not help much
	Mat mean1;
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

	Mat gray, binary, binaryBlured;
	cv::cvtColor(src, gray, CV_RGB2GRAY);

	//1) Prepare an image to use as a mask
	//BINARY FROM GRAY
	int binaryImageThreshold = 130;
	createBinaryImage(gray, binary, binaryImageThreshold, 255);
	//imshow("Binary", binary);

	//BLUR BINARY
	cv::medianBlur(binary, binaryBlured, 3);
	//imshow("Blur Binary", binaryBlured);

	//DILATE BLURED BINARY
	cv::Mat dilated;  // Result output
	cv::dilate(binaryBlured, dilated, Mat(), Point(-1, -1), 2, 1, 1);
	//Show Dialted
	//imshow("Dialted", dilated);

	cv::Mat filled = dilated.clone();
	cv::floodFill(filled, Point(filled.size().width / 4, filled.size().height / 2), cvScalar(255, 255, 255));
	//imshow("Filled", filled);

	// 3) Mask an image with edges using an image created on step 1)
	Mat masked;
	gray.copyTo(masked, filled);

	//imshow("Canny_Masked", masked);

	dst = masked.clone();
}

void MarkDetectedPlayer(Mat& img, Rect& detectedRect) {
	Rect r = rectForCircle(detectedRect);
	cv::rectangle(img, r, Scalar(255, 0, 0), 1);
	drawCircle(img, r);
}
