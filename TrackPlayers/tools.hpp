#pragma once

#include <opencv2/opencv.hpp>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace std;
using namespace cv;


void drawCircle(cv::Mat& img, cv::Rect& r, int scale = 1, cv::Scalar color = Scalar(0,0,255), int thinckness = CV_FILLED) {
	cv::Point center(r.x + r.width / 2, r.y+r.height/2);
	//double hight = r.width * 0.38;
	const cv::RotatedRect rect(center, Size(r.width*0.7, r.height*0.5), 0);
	cv::ellipse(img, rect, color, thinckness);
}

Rect rectForCircle(cv::Rect r) {
	return Rect((r.x + r.width*0.0625), (r.y + r.height - (r.width*0.9*0.75)), r.width*0.9, r.width*0.9*0.75);
}

cv::Rect scaleRect(cv::Rect& r, double scale) {
	return Rect((r.x*scale), (r.y*scale), (r.width*scale), (r.height*scale));
}


void makeGray(cv::Mat& src, cv::Mat& dst) {
	cv::cvtColor(src, dst, CV_RGB2GRAY);
}


/*
threshold the image by setting all pixel values less than 225
to 255 (white; foreground) and all pixel values >= 225 to 255
(black; background), thereby segmenting the image
*/
cv::Mat createBinaryImage(cv::Mat& src, int threshold, double maxVal) {
	Mat dst;
	cv::threshold(src, dst, threshold, maxVal, THRESH_BINARY);
	return dst;
}


void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal) {
	cv::threshold(src, dst, threshold, maxVal, THRESH_BINARY_INV);
}

void addContrast(Mat& src, Mat& dst, double contrast = 0) {
	if (contrast != 0) {
		double f = 131.0 * (contrast + 127.0) / (127.0 * (131.0 - contrast));
		double alpha_c = f;
		double gamma_c = 127 * (1 - f);
		cv::addWeighted(src, alpha_c, src, 0, gamma_c, dst);
	}
}

void applyMask(Mat& src, Mat& dst, Mat& mask)
{
	src.copyTo(dst, mask);
}
