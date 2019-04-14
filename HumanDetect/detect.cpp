#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"
#include "Haar_Cascade_Classifier.cpp"
#include "HOGDetector.cpp"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;


//draws detected (rectangles) over the image
void DrawDetected(Mat& frame, std::vector<Rect>& detected, Scalar color = Scalar(0, 0, 255), int thinkness = 1);
//saves an image to a file
bool save(Mat& frame, string toFile);

int haasClassifierDetect(vector<string> fileName);
int HOGPeopleDefaultDetect(vector<string> images);
void testHOG(string imageName);


int detectOnImages(int argc, char** argv) {
	if (argc < 2)
		cerr << "Please enter a filename" << endl;

	CSVReader fileNamesReader(argv[1]);
	fileNamesReader.readLines();
	vector<string> fileNames = fileNamesReader.getLines();

	if (fileNames.size() == 0) {
		cerr << "Received 0 filenames." << endl;
		return 1;
	}

	int hogTest = HOGPeopleDefaultDetect(fileNames);
	int haasTest = haasClassifierDetect(fileNames);
	return hogTest && haasTest;
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
int HOGPeopleDefaultDetect(vector<string> images) {
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	std::ofstream results;
	results.open("HOGDetection-Results.csv");
	results << "FileName, x_y_width_height(weight)" << endl;

	double progress = 0;
	int imgCount = 0;
	int totalImages = images.size();
	cout << "HOG People Detect started:\n";
	for (string imageName : images) {
		imgCount++;
		progress = (double)imgCount / (double)totalImages;
		cout << "Progress: " << progress*100 << "% (" << imgCount << "/" << totalImages << ")" << endl;
		Mat img = cv::imread(imageName);
		if (img.empty()) {
			cerr << "could not open image named '" << imageName << ",\n";
		}
		vector<Rect> detected;
		vector<double> weights;
		double hitThreshold = 0;
		Size winStride = Size(8, 8);
		Size padding = Size(32, 32);
		double scale = 1.05;
		hog.detectMultiScale(img, detected, weights, hitThreshold, winStride, padding, scale, 2, false);

		results << imageName << ",";
		//write detected rectangles for this file. 
		//output file example
		// imageName.jpg, x_y_width_height,  x_y_width_height,  x_y_width_height, 
		for(size_t idx =0; idx < detected.size(); idx++)
			results << detected[idx].x << "_" << detected[idx].y << "_" << detected[idx].width << "_" << detected[idx].height << ",";
		
		cv::Mat out = img;
		DrawDetected(out, detected);
		std::size_t dotIdx = imageName.find_last_of(".");
		string outFileNameWithoutExt = imageName.substr(0, dotIdx);
		size_t slashIdx = imageName.find_last_of("/");
		string outFileName = outFileNameWithoutExt.substr(slashIdx + 1);
		string outFileExt = imageName.substr(dotIdx);
		outFileName += "-out-HOG" + outFileExt;
		save(out, outFileName);
		results << endl;
	}
	
	return 0;
}
int haasClassifierDetect(vector<string> imageFileNames) {

	string classifierFileName = "Support/haarcascade_fullbody.xml";
	HaarBodyClassifier haasClassifier(classifierFileName);

	
	if (haasClassifier.status)
	{
		
		std::ofstream results;
		results.open("HaasDetection-Results.csv");
		results << "FileName,Coordinates" << endl;
		double progress = 0;
		int imgCount = 0;
		int totalImages = imageFileNames.size();
		cout << "HOG People Detect started:\n";
		for (string imageName : imageFileNames) {
			imgCount++;
			progress = (double)imgCount / (double)totalImages;
			cout << "Progress: " << progress * 100 << "% (" << imgCount << "/" << totalImages << ")" << endl;
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
				DrawDetected(out, detected);

				std::size_t dotIdx = imageName.find_last_of(".");
				string outFileNameWithoutExt = imageName.substr(0, dotIdx);
				size_t slashIdx = imageName.find_last_of("/");
				string outFileName = outFileNameWithoutExt.substr(slashIdx+1);
				string outFileExt = imageName.substr(dotIdx);
				outFileName += "-out-HAAR" + outFileExt;
				save(out, outFileName);
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


// This method displays an example of HOG detector on 1 image using different parameters
void testHOG(string imageName) {
	HOGDetector detector(HOGDetectorMode::Default);

	Mat img = imread(imageName);
	Mat outAsDetected;
	Mat outAdjusted;
	for (;;) {
		vector<Rect> detected = detector.detect(img);
		vector<Rect> detectedAdjusted;
		
		outAsDetected = img.clone();
		outAdjusted = img.clone();

		string description = detector.description();

		DrawDetected(outAsDetected, detected);
		putText(outAsDetected, description, Point(10, 30), FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255), 1, LINE_AA);
		imshow("Out As Detected", outAsDetected);

		for (auto& d : detected)
			detectedAdjusted.push_back(detector.adjustRect(d));

		DrawDetected(outAdjusted, detectedAdjusted);
		putText(outAdjusted, description, Point(10, 30), FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255), 1, LINE_AA);
		imshow("Out Adjusted", outAdjusted);


		char option = waitKey(40);
		if (option == 72) //Upper H(itThreshold)++
		{
			detector.hitThreshold += 0.5;
			cout << "hitThreshold: " << detector.hitThreshold << endl;
		}
		else if (option == 104) //Lowercase h(itThreshold)--
		{
			detector.hitThreshold -= 0.5;
			cout << "detector.hitThreshold: " << detector.hitThreshold << endl;
		}
		else if (option == 87) //Upper W(hitThreshold)++
		{
			detector.winStride = Size(detector.winStride.width+1, detector.winStride.height+1);
			cout << "detector.winStride: " << detector.winStride << endl;
		}
		else if (option == 119) //Lowercase w(inStride)++
		{
			detector.winStride = Size(detector.winStride.width-1, detector.winStride.height-1);
			cout << "detector.winStride: " << detector.winStride << endl;
		}
		else if (option == 80) //Upper p(adding)++ 
		{
			detector.padding = Size(detector.padding.width+1, detector.padding.height+1);
			cout << "detector.padding: " << detector.padding << endl;
		}
		else if (option == 112) //Lowercase p(adding)--
		{
			detector.padding = Size(detector.padding.width-1, detector.padding.height-1);
			cout << "detector.padding: " << detector.padding << endl;
		}
		else if (option == 83) //Upper S(caleFactor)++ 
		{
			detector.scale += 0.05;
			cout << "scaleFdetector.scaleactor: " << detector.scale << endl;
		}
		else if (option == 115) //Lowercase s(caleFactor)--
		{
			detector.scale -= 0.05;
			cout << "detector.scale: " << detector.scale << endl;
		}
		else if (option == 81) //Lowercase Q(uit)
		{
			break;
		}
		outAdjusted.release();
		outAsDetected.release();
	}
	



}