#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"
#include "Haar_Cascade_Classifier.cpp"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

bool debug = true;
//Methods


int dirtyCode(int argc, char** argv) {
	if (argc < 2) {
		cerr << "Please enter a filename" << endl;
		return 0;
	}
	string classifierFileName = "Support/haarcascade_fullbody.xml";
	HaarBodyClassifier haasClassifier(classifierFileName);

	Mat img = cv::imread(argv[1]);
	if (img.empty()) {
		cerr << "could not open image named '" << argv[1] << ",\n";
	}

	cv::resize(img, img, Size(), 0.5, 0.5);
	
	string windowName = "Haar Casscade full body";
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

	char option = waitKey(40);
	double scaleFactor = 1.0;
	int minNeighbours = 2;
	int flags = 0 | CV_HAAR_SCALE_IMAGE;
	Size minSize = Size(30, 30);
	
	Mat out = img.clone();
	//cvtColor(img, out, CV_BGR2GRAY);
	//equalizeHist(out, out);
	for (;;) {
		haasClassifier.scaleFactor = scaleFactor;
		haasClassifier.minNeighbours = minNeighbours;
		haasClassifier.minSize = minSize;
		vector<Rect> detected = haasClassifier.detectAuto(img);
		if (detected.size() > 0) {
			for (int gg = 0; gg < detected.size(); gg++) {
				rectangle(out, detected[gg].tl(), detected[gg].br(), Scalar(0, 0, 255), 1, 8);
			}
		}
		cv::imshow(windowName, out);

		if (option == 77) //Upper M(inSize)++
		{
			minSize.width += 1.0;
			minSize.height += 1.0;
			cout << "minSize: " << minSize << endl;
		}
		else if (option == 109) //Lowercase m(inSize)--
		{
			minSize.width -= 1.0;
			minSize.height -= 1.0;
			cout << "minSize: " << minSize << endl;
		}
		else if (option == 78) //Upper N(eighbours)++
		{
			minNeighbours += 1;
			cout << "minNeighbours: " << minNeighbours << endl;
		}
		else if (option == 110) //Lowercase n(eighbours)--
		{
			minNeighbours -= 1;
			cout << "minNeighbours: " << minNeighbours << endl;
		} else if (option == 83) //Upper S(caleFactor)++ 
		{
			scaleFactor += 0.1;
			cout << "scaleFactor: " << scaleFactor << endl;
		}
		else if (option == 115) //Lowercase s(caleFactor)--
		{
			scaleFactor -= 0.1;
			cout << "scaleFactor: " << scaleFactor << endl;
		}
		else if (option == 81) //Lowercase Q(uit)
		{
			break;
		}
		option = waitKey(40);
	}
	return 0;
}
