#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace std;
using namespace cv;

//image for the circle to be drawn on (upscaled image), rectangle from downscaled image, scale (ie if original is shrunk by diving by 3 scale = 3)
void drawCircle(cv::Mat img, cv::Rect r, int scale) {
	circle(img, Point((r.x+ (r.width / 2))*scale, (r.y+(r.height/1.25))*scale), (r.width / 2.5)*scale, Scalar(50, 255, 210), 2);
}

Rect rectForCircle(cv::Rect r) {
	return Rect((r.x + r.width*0.0625), (r.y + r.height - (r.width*0.9*0.75)), r.width*0.9, r.width*0.9*0.75);
}

cv::Rect scaleRect(cv::Rect& r, double scale) {
	return Rect((r.x*scale), (r.y*scale), (r.width*scale), (r.height*scale));
}
/*
int drawcirclemain(int argc, char** argv) {

	if (argc < 1) {
		return 0;
	}

	std::string groupName = "Project";

	//image1
	cv::Mat img1 = cv::imread(argv[1], -1);
	if (img1.empty())
	{
		cout << "Could not read the image at the specified path." << endl;
		return -1;
	}

	cv::namedWindow(groupName + ". Original", cv::WINDOW_AUTOSIZE);
	cv::imshow(groupName + ". Original", img1);

	int width = img1.cols;
	int height = img1.rows;
	//image will be 3 times smaller
	int scale = 3;

	//	Convert the image to gray scale 
	cv::Mat img2;

	//Scale = 3;
	cv::resize(img1, img2, cv::Size(int(width / scale), int(height / scale)));

	//rectangle params, x,y,width,height
	int rx = 100;
	int ry = 80;
	int rw = 50;
	int rh = 80;

	Rect r = Rect(rx, ry, rw, rh);

	rectangle(img2, r, Scalar(10, 255, 10));

	//circle(img2, Point(rx+(r.width/2), ry+(r.height/1.25)), r.width / 2.5, Scalar(100, 55, 210), 2);

	cv::Rect r2 = rectForCircle(r);

	rectangle(img2, r2, Scalar(50, 50, 50));

	cv::namedWindow(groupName + ". Gray", cv::WINDOW_AUTOSIZE);
	cv::imshow(groupName + ". Gray", img2);

	waitKey();

	//after growth
	cv::Mat img3 = img1;
	cv::Rect r3 = scaleRect(r2, scale);
	rectangle(img3, r3, Scalar(150, 0, 255));
	cv::namedWindow(groupName + ". scaled", cv::WINDOW_AUTOSIZE);
	cv::imshow(groupName + ". scaled", img3);
	
	waitKey();

	destroyAllWindows();

	return 0;
}
*/