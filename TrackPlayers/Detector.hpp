#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include "HOGDetector.hpp"
#include "tools.hpp"

using namespace cv;

class Detector
{
private:
	cv::HOGDescriptor hog;
	void MakeMask(cv::Mat& src, cv::Mat& dst);
	void PrepareForProcessing(Mat& src, Mat& dst, int colorSpace, double contrast, int option);
public:
	double hitThreshold;
	//Window stride.It must be a multiple of block stride
	cv::Size winStride;
	//coefficient of the detection window increase.
	cv::Size padding;
	//scaleFactor - Parameter specifying how much the image size is reduced at each image scale.
	double scale;
	//group_threshold – Coefficient to regulate the similarity threshold. When detected, some objects can be covered by many rectangles. 0 means not to perform grouping. See groupRectangles() .
	double group_threshold;
	std::vector<cv::Rect> detected;
	std::vector<double> weights;

	Detector(cv::InputArray detector = cv::HOGDescriptor::getDefaultPeopleDetector());
	void Detect(cv::Mat & srcImage, Mat& dst);
	void DetectWithPreprocessing(cv::Mat& srcImage);
	~Detector();

};

inline Detector::Detector(cv::InputArray detector)
{
	hog.setSVMDetector(detector);
	hitThreshold = 0;
	winStride = cv::Size(6, 6);
	padding = cv::Size(16, 16);
	scale = 1.05;
	group_threshold = 2;
}

void Detector::Detect(cv::Mat & srcImage, Mat& dst )
{
	hog.detectMultiScale(srcImage, detected, weights, hitThreshold, winStride, padding, scale, group_threshold, false);
}

 void Detector::DetectWithPreprocessing(cv::Mat & srcImage)
{
	 Mat mask, preparedImage, preparedImageMasked;
	 PrepareForProcessing(srcImage, preparedImage, COLOR_RGB2YUV, 10, 2);

	 MakeMask(srcImage, mask);
	 applyMask(preparedImage, preparedImageMasked, mask);
	 
	 hog.detectMultiScale(preparedImageMasked, detected, weights, hitThreshold, winStride, padding, scale, group_threshold, false);
}

 /*
 Algorithm:
 The rink has a lot of empty space, so if we threshold with a big kernel we can get rid of everything on a rink and use it as a mask
 1) Add contrast
 2) Convert to grayscale
 3) Threshold gray image
	 Use big kernel size to remove players and noise from the rink
 4) Apply morph opening to remove small details
 5) Blur the image
	 Use high value, like 25
 6) Dilate to remove some big chunks
 7) Erode to return to get back lost area of the rink
 9) Use bitwise flip to get a mask
 */
 void Detector::MakeMask(Mat& src, Mat& dst) {

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
	 int kernelSize = 35; //kernel size
	 cv::Mat elem = 255 * cv::Mat::ones(kernelSize, kernelSize, CV_8UC1); // 19x19 kernel matrix filled with 255
	 cv::morphologyEx(binary, binary, MORPH_OPEN, elem);

	 // 5) BLUR BINARY
	 cv::medianBlur(binary, binary, 25);
	 //imshow("Blur Binary", binaryBlured);

	 // 6) Open blured, but use dilate + erode not open function
	 cv::dilate(binary, binary, elem);
	 cv::dilate(binary, binary, elem);

	 // 6) Erode
	 int erodekernelSize = 10; //kernel size
	 cv::Mat erodeelem = 255 * cv::Mat::ones(erodekernelSize, erodekernelSize, CV_8UC1); // 19x19 kernel matrix filled with 255
	 cv::erode(binary, binary, erodeelem);
	 cv::bitwise_not(binary, dst);
 }

 void Detector::PrepareForProcessing(Mat& src, Mat& dst, int colorSpace, double contrast, int option) {

	 //cv::cvtColor(in, out, colorSpace);

	 if (option == 1) {
		 addContrast(src, dst, contrast);

		 Mat imageHSV, diff;
		 cv::cvtColor(src, imageHSV, COLOR_RGB2HSV);

		 absdiff(src, imageHSV, diff);

	 }
	 else if (option == 2) {
		 Mat contrasted, gray, dilated;
		 //Add contrast
		 addContrast(src, contrasted, contrast);
		 //Make gray
		 cv::cvtColor(contrasted, gray, COLOR_RGB2GRAY);
		 //	Dilate
		 int kernelSize = 15; //kernel size
		 cv::Mat elem = 255 * cv::Mat::ones(kernelSize, kernelSize, CV_8UC1); // 15x15 kernel matrix filled with 255
		 dilate(gray, dilated, elem);
		 // BLUR
		 cv::medianBlur(dilated, dilated, 17);
		 cvtColor(dilated, dilated, COLOR_GRAY2BGR);
		//Get difference
		 absdiff(src, dilated, dst);

	 }
	 else {

		 Mat imageHSV, diff;
		 cv::cvtColor(src, imageHSV, COLOR_RGB2HSV);
		 absdiff(src, imageHSV, diff);
		 absdiff(imageHSV, diff, dst);

		 double sp = 10.0, sr = 20.0;
		 int maxLevel = 1;
		 cv::pyrMeanShiftFiltering(diff, dst, sp, sr, maxLevel);

	 }
 }

Detector::~Detector()
{
}
