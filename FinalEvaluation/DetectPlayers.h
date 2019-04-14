#pragma once 
#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"
#include "../TrackPlayers/HogDetector.hpp"
#include "../TrackPlayers/Detector.hpp"
#include "../TrackPlayers/tools.hpp"

void DrawDetected(Mat& frame, std::vector<Rect>& detected, Scalar color = Scalar(0, 0, 255), int thinkness = 1) {
	if (detected.size() > 0) {
		for (int gg = 0; gg < detected.size(); gg++) {
			rectangle(frame, detected[gg].tl(), detected[gg].br(), color, thinkness, 8);
		}
	}
}


/*
THIS PIECE OF CODE DETECTS PLAYERS ON ALL IMAGES INSIDE TEST FOLDER
SAVES DETECTED RECTANGLES TO A FILE READY TO BE USED BY EVALUATION CODE
*/

bool DetectOnAllImages(string fileWithImagePaths, string resultsFileName) {

	CSVReader fileNamesReader(fileWithImagePaths);
	fileNamesReader.readLines();
	vector<string> fileNames = fileNamesReader.getLines();

	if (fileNames.size() == 0) {
		cerr << "File with images cannot be read." << endl;
		return false;
	}

	std::ofstream results;
	results.open(resultsFileName);
	results << "FileName, x_y_width_height(weight)" << endl;

	Detector d;
	int totalImages = fileNames.size();
	int imgCount = 0;
	double progress = 0;
	for (string imageName : fileNames) {
		imgCount++;
		
		Mat img = cv::imread(imageName);
		if (img.empty()) {
			cerr << "could not open image named '" << imageName << ",\n";
			continue;
		}
		else {
			//DETECT PLAYERS FOR THIS IMAGE
			d.DetectWithPreprocessing(img);

			//write detected rectangles for this file. 
			//output file example
			// imageName.jpg, x_y_width_height,  x_y_width_height,  x_y_width_height, 
			results << imageName << ",";
			for (size_t idx = 0; idx < d.detected.size(); idx++)
				results << d.detected[idx].x << "_" << d.detected[idx].y << "_" << d.detected[idx].width << "_" << d.detected[idx].height << ",";

			cv::Mat out = img.clone();
			DrawDetected(out, d.detected);

			std::size_t dotIdx = imageName.find_last_of(".");
			string outFileNameWithoutExt = imageName.substr(0, dotIdx);
			size_t slashIdx = imageName.find_last_of("/");
			string outFileName = outFileNameWithoutExt.substr(slashIdx + 1);
			string outFileExt = imageName.substr(dotIdx);
			outFileName += "-out" + outFileExt;
			imwrite(outFileName, out);
			results << endl;
		}
		progress = (double)imgCount / (double)totalImages;
		cout << "Progress: " << progress * 100 << "% (" << imgCount << "/" << totalImages << ")" << endl;
	}
	results.close();
	return true;
}