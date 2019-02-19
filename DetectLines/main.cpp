#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

void help(char** argv) {
	std::cout << "\n"
		<< "2.2: Like 2.1, but 'using namespace cv: \n"
		<< argv[0] << " <path/image>\n"
		<< "For example:\n"
		<< argv[0] << " ../fruits.jpg\n"
		<< std::endl;
}

///This method turns an image into a grayscale image 
void makeGray(cv::Mat& src, cv::Mat& dst);

///a method to create a binary image from an input image
cv::Mat createBinaryImage(cv::Mat& src, int threshold, double maxVal);
void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal = 255);

template <class T>
void askForInput(string& message, T& value) {
	cout << message;
	cin >> value;
}


int main(int argc, char** argv) {

	if (argc < 2) {
		help(argv);
		return 0;
	}

	std::string groupName = "Group 4";

	//READ IMAGE
	cv::Mat originalImage = imread(argv[1], -1);
	if (originalImage.empty())
	{
		cout << "Could not read the image at the specified path." << endl;
		return -1;
	}
	string windowName = groupName;

	cv::Mat gray, binary;
	makeGray(originalImage, gray);

	imshow(windowName, gray);

	int threshold=100;
	double maxVal = 255;


	for (;;){
		char option = waitKey(40);

		if (option == 84) //Capital T
		{	
			threshold+=5;
			cout << "Binary: threshold: " << threshold << ", maxVal: " << maxVal << endl;
		}if (option == 116) //Lowercase t
		{
			threshold-=5;
			cout << "Binary: threshold: " << threshold << ", maxVal: " << maxVal << endl;
		}
		if (option == 77) //Capital M
		{
			maxVal+=5;
			cout << "Binary: threshold: " << threshold << ", maxVal: " << maxVal << endl;
		}
		if (option == 109) //Lowercase m
		{
			maxVal-=5;
			cout << "Binary: threshold: " << threshold << ", maxVal: " << maxVal << endl;
		}
		if (option == 13)
			break;
		createBinaryImage(gray, binary, threshold);
		imshow(windowName, binary);
	}

	waitKey();

	destroyAllWindows();
	return 0;
}


void makeGray(cv::Mat& src, cv::Mat& dst) {
	cv::cvtColor(src,dst,CV_RGB2GRAY);
}

cv::Mat createBinaryImage(cv::Mat& src, int threshold, double maxVal){
	Mat dst;
	cv::threshold(src,dst,threshold, maxVal, THRESH_BINARY);
	return dst;
}

void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal) {
	cv::threshold(src, dst, threshold, maxVal, THRESH_BINARY);
}

