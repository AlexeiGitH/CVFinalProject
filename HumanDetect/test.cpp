#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"
#include "Haar_Cascade_Classifier.cpp"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;


//draws detected (rectangles) over the image
void DrawDetected(Mat& frame, std::vector<Rect>& detected, Scalar color = Scalar(0, 0, 255), int thinkness = 1);
//saves an image to a file
bool save(Mat& frame, string toFile);

int haasClassifierTest(string fileName);

int main(int argc, char** argv) {
	if (argc < 2)
		cerr << "Please enter a filename" << endl;

	return haasClassifierTest(argv[1]);
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

//TESTS
int haasClassifierTest(string fileName) {


	CSVReader fileNamesReader(fileName);
	fileNamesReader.readLines();
	vector<string> fileNames = fileNamesReader.getLines();

	if (fileNames.size() == 0) {
		cerr << "Received 0 filenames." << endl;
		return 1;
	}

	string classifierFileName = "Support/haarcascade_fullbody.xml";
	HaarBodyClassifier haasClassifier(classifierFileName);

	if (haasClassifier.status)
	{
		std::ofstream results;
		results.open("HaasDetection-Results.csv");
		results << "FileName,Coordinates" << endl;
		for (string imageName : fileNames) {
			Mat img = cv::imread(imageName);
			if (img.empty()) {
				cerr << "could not open image named '" << imageName << ",\n";
			}
			else {
				vector<Rect> detected = haasClassifier.detect(img);
				results << imageName << ",";
				//write detected rectangles for this file. 
				//output file example
				// imageName.jpg, x_y_width_height,  x_y_width_height,  x_y_width_height, 
				for (auto d : detected)
					results << d.x << "_" << d.y << "_" << d.width << "_" << d.height << ",";
				cv::Mat out = img;
				DrawDetected(img, detected);

				std::size_t dotIdx = imageName.find_last_of(".");
				string outFileNameWithoutExt = imageName.substr(0, dotIdx);
				size_t slashIdx = imageName.find_last_of("/");
				string outFileName = outFileNameWithoutExt.substr(slashIdx+1);
				string outFileExt = imageName.substr(dotIdx);
				outFileName += "-out" + outFileExt;
				save(img, outFileName);
				results << endl;
			}
		}
		results.close();
	}
	else {
		cerr << "Could not create a classifier." << endl;
		return 2;
	}
	return 0;
}