#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "DetectPlayers.h"
#include "PrecisionCalc.h"
#include "../Shared/CSVReader.h"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

std::map<string, vector<cv::Rect>> parseAlgFileData(vector<string> data);
std::map<string, vector<cv::Point>> parseGTFileData(vector<string> data);
int main(int argc, char** argv) {

	if (argc != 3)
	{
		cerr << "Expecting 2 arguments, 1 - file with the list of images, 2 - file with GT for images\n";
		return 1;
	}

	
	//File with paths to images
	string imagePaths = argv[1];
	//Detect players using our algorithm and save results (rectangles for images) to the file, so we can use it to compare
	//if (!DetectOnAllImages(imagePaths, "detected.csv"))
		//return 2;
		
	
	//Load all ground truth data
	CSVReader gtData(argv[2]);
	gtData.readLines('\n');
	vector<string> gtLines = gtData.getLines();
	if (gtLines.size() == 0)
	{
		cerr << "GT file is empty\n";
		return 3;
	}
	std::map<string, vector<cv::Point>> gtfilesWithPoints = parseGTFileData(gtLines);
	

	//LOAD DETECTED DATA
	CSVReader detectedRectanges("detected.csv");
	detectedRectanges.readLines('\n');
	vector<string> algHaarLines = detectedRectanges.getLines();
	if (algHaarLines.size() == 0)
	{
		cerr << "Alg(HaarResults) file is empty\n";
		return 2;
	}
	std::map<string, vector<cv::Rect>> detectedfilesWithRects = parseAlgFileData(algHaarLines);
	

	//COMPARE DETECTED RECTS WITH GT POINTS ON PLAYERS
	std::ofstream results;
	results.open("PerformanceResults.csv");
	results << "FileName, P, R, F1, TP, FP, FN" << endl;
	//test data
	for (auto &entity : gtfilesWithPoints) {
		const string& fileName = entity.first;
		//find rects for this image
		auto iterator = detectedfilesWithRects.find(fileName);
		if (iterator != detectedfilesWithRects.end()) {
			vector<Rect>& rects = iterator->second;
			PrecisionCalc p;
			Point* points = entity.second.data();
			int pointsArraySize = entity.second.size();

			Rect* rectsArray = rects.data();
			int rectsArraySize = rects.size();
			p.calculateAccuracy(points, rectsArray, pointsArraySize, rectsArraySize);
			results << fileName << "," << p.P << "," << p.R << "," << p.F
				<< "," << p.TP << "," << p.FP << "," << p.FN << endl;
		}
		else {
			std::cout << fileName <<" image has GT points but not found in detected\n";
		}
	}
	return 0;
}

std::vector<std::string> split(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}

std::map<string, vector<cv::Point>> parseGTFileData(vector<string> data) {

	std::map<string, vector<cv::Point>> filesWithPoints;
	for (string& line : data)
	{
		vector<string> splitLine = split(line, ',');
		int elements = splitLine.size();
		string fileName = "";
		vector<cv::Point> points;

		if (elements > 2)
		{
			for (size_t i = 1; i < elements; i++)
			{
				string elementValue = splitLine[i];
				vector<string> xyString = split(elementValue, ';');
				if (xyString.size() == 2) {
					//convert to Points
					points.push_back(Point(stoi(xyString[0]), stoi(xyString[1])));
				}
			}
		}
		if (elements >= 1)
		{
			//first one is a file name. 
			fileName = splitLine[0];
			filesWithPoints[fileName] = points;
		}

	}
	return filesWithPoints;
}
std::map<string, vector<cv::Rect>> parseAlgFileData(vector<string> data) {
	std::map<string, vector<cv::Rect>> filesWithRects;

	int size = data.size();
	for (size_t i = 1; i < size; i++)
	{
		//start from line 2 since line 1 is a header
		string& line = data[i];
		vector<string> splitLine = split(line, ',');
		int elements = splitLine.size();
		string fileName = "";
		vector<cv::Rect> rects;

		if (elements > 2)
		{
			for (size_t i = 1; i < elements; i++)
			{
				string elementValue = splitLine[i];
				vector<string> xyString = split(elementValue, '_');
				if (xyString.size() == 4) {
					//convert to Points
					rects.push_back(Rect(stoi(xyString[0]), stoi(xyString[1]), stoi(xyString[2]), stoi(xyString[3])));
				}
			}
		}
		if (elements >= 1)
		{
			//first one is a file name
			fileName = splitLine[0];
			filesWithRects[fileName] = rects;
		}

	}


	return filesWithRects;
}
