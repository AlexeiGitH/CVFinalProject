#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "CSVReader.h"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

bool debug = true;
//global variables
std::map<string, std::vector<cv::Point>> pointsForFiles;
std::vector<cv::Point> currentFilePoints;

void callbackButton(int state, void *pointer) {
	cout << "button clicked: " << state << endl;
}
void my_mouse_callback(int event, int x, int y, int flags, void* param);
void drawCircle(Mat image, Point point);

//name of the file from where list of omages was read
void exit(string fileName);

int main(int argc, char** argv) {
	string groupName = "Group 4";

	string fileName = "images-all.txt";
	CSVReader fileReader(fileName);
	fileReader.readLines();

	//Array of files
	vector<string> files = fileReader.getLines();

	string windowName = groupName + ". Annotate";
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

	cv::Mat img1;

	int nFiles = files.size();
	int fileIdx = 0;

	if (nFiles == 0)
	{  
		cerr << "Could not load file names";
		return 1;
	}
	img1 = cv::imread(files[fileIdx]);

	setMouseCallback(windowName, my_mouse_callback, (void*)&img1);
	currentFilePoints.clear();
	char option = waitKey(40);
	for (;;) {
		if (!img1.empty())
			cv::imshow(windowName, img1);

		if (option == 78) //Capital N(ext)
		{
			if (fileIdx < nFiles - 1) {
				fileIdx += 1;
				img1 = cv::imread(files[fileIdx]);
				pointsForFiles[files[fileIdx]] = currentFilePoints;
				currentFilePoints.clear();
			}
			else {
				pointsForFiles[files[fileIdx]] = currentFilePoints;
				break;
			}
		}
		//if (option == 80) //Lowercase P(revious)
		//{
		//	if (fileIdx > 0) {
		//		fileIdx-=1;
		//		img1 = cv::imread(files[fileIdx], cv::IMREAD_GRAYSCALE);
		//	}
		//}
		if (option == 81) //Lowercase Q(uit)
		{
			pointsForFiles[files[fileIdx]] = currentFilePoints;
			break;
		}
		option = waitKey(40);
	}
	cv::destroyWindow(windowName);
	exit(fileName);
	return 0;
}

void drawCircle(Mat img, Point point) {
	//circle radius is the smallest value of width or height
	int radius = 5;
	Scalar color(255, 0, 255);
	int thickness = 1;
	cv::circle(img, point, radius, color, CV_FILLED, thickness);
	currentFilePoints.push_back(point);
}
void my_mouse_callback(int event, int x, int y, int flags, void* param) {
	cv::Mat& image = *(cv::Mat*) param;
	switch (event) {
		if (debug)
			cout << "Mouse click at location: " << x << "," << y << endl;
	case cv::EVENT_LBUTTONDOWN: { drawCircle(image, cv::Point(x, y)); } break;
	}
}

void exit(string fileName) {
	std::ofstream myfile;

	std::size_t found = fileName.find_last_of(".");
	string fName = fileName.substr(0, found);
	fName += "-pointsForFiles.csv";
	myfile.open(fName);
	for (auto const &x : pointsForFiles) {
		auto const &fileName = x.first;
		auto const &points = x.second;
		myfile << fileName << ",";
		for (auto const& point : points)
			myfile << point.x << ";" << point.y << ",";
		myfile << "\n";
	}
	myfile.close();
}