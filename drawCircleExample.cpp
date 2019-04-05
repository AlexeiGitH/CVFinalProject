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

int main(int argc, char** argv) {

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

	circle(img2, Point(rx+(r.width/2), ry+(r.height/1.25)), r.width / 2.5, Scalar(100, 55, 210), 2);

	cv::namedWindow(groupName + ". Gray", cv::WINDOW_AUTOSIZE);
	cv::imshow(groupName + ". Gray", img2);

	waitKey();

	//	Apply histogram equalization 
	cv::Mat img3 = img1;
	drawCircle(img3, r, scale);
	cv::namedWindow(groupName + ". circled", cv::WINDOW_AUTOSIZE);
	cv::imshow(groupName + ". circled", img3);
	
	waitKey();

	destroyAllWindows();

	return 0;
}