#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

class cutVideo {

public:
	bool cut5Seconds(string filename, cv::VideoCapture cap);
};