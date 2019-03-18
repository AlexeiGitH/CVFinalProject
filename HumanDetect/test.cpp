#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "../Shared/CSVReader.h"


#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;


//draws detected (rectangles) over the image
void DrawDetected(Mat& frame, std::vector<Rect>& detected, Scalar color = Scalar(0, 0, 255), int thinkness = 1);
//saves an image to a file
bool save(Mat& frame, string toFile);


std::vector<std::string> split(std::string strToSplit, char delimeter);
std::map<string, vector<cv::Point>> parseGTFileData(vector<string> data);
std::map<string, vector<cv::Rect>> parseAlgFileData(vector<string> data);

int main(int argc, char** argv) {
	/*if (argc < 2)
		cerr << "Please enter a filename" << endl;*/

	//Load GT
	CSVReader gtData("images-test-pointsForFiles.csv");
	gtData.readLines('\n');
	vector<string> gtLines = gtData.getLines();
	if (gtLines.size() == 0)
	{
		cerr << "GT file is empty\n";
		return 1;
	}
	std::map<string, vector<cv::Point>> gtfilesWithPoints = parseGTFileData(gtLines);

	//Load agl1 data
	CSVReader algHaarData("HaasDetection-Results.csv");
	algHaarData.readLines('\n');
	vector<string> algHaarLines = algHaarData.getLines();
	if (algHaarLines.size() == 0)
	{
		cerr << "Alg(HaarResults) file is empty\n";
		return 2;
	}
	std::map<string, vector<cv::Rect>> alg1filesWithRects = parseAlgFileData(algHaarLines);


	//test data


	return 0;
}

std::map<string, vector<cv::Point>> parseGTFileData(vector<string> data) {
	
	std::map<string, vector<cv::Point>> filesWithPoints;
	for (string& line: data)
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