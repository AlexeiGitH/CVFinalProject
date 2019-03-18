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

	void calculateAccuracy(cv::Point *p, cv::Rect *r, int numPoint, int numRect) {
		bool found = false;
		int FP = 0;
		int FN = 0;
		int TP = 0;
		int TN = 0;
		int total = numPoint;

		//for every rectangle, check if there is a point inside
		//if true, add a true positive and stop checking (prevents multiple points from adding multiple TP for a single rect)
		//if no point is found inside the rect, add a false negative
		for (int j = 0; j < numRect; j++) {
			for (int k = 0; k < numPoint; k++) {
				if (detectPoint(p[k], r[j])) {
					if (!found) {
						TP++;
						found = true;
					}
				}
			}
			if (!found) {
				FP++;
			}

		}

		//reset found
		found = false;

		//for every point, check if there is a rectangle that bounds the point
		//if no rectangle is found, add a false negative
		for (int x = 0; x < numPoint; x++) {
			for (int y = 0; y < numRect; y++) {
				if (detectPoint(p[x], r[y])) {
					found = true;
				}
			}
			if (!found) {
				FN++;
			}
		}

		double R = TP / (TP + FN);

		double P = TP / (TP + FP);

		double F = (1 + 1)*((P*R) / ((1 * P) + R));

	}
};