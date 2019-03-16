#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

class detectRectangle {

public:
	bool detectPoint(cv::Point p, cv::Rect r) {
		//if the point is horizontally within the rectangle
		if (p.x >= r.x && p.x <= (r.x + r.width)) {
			//if the point is vertically withing the rectangle
			if (p.y >= r.y && p.y <= r.y + r.height) {
				return true;
			}
		}
		return false;
	}

	//is there any intersection at all, the 0 could be increased for a better match, depends on the area of the rectangles
	bool detectRectOverlap(cv::Rect r1, cv::Rect r2) {
		return ((r1 & r2).area() > 0);
	}
};