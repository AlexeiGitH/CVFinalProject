#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

class PrecisionCalc {

public:
	double R, P, F;
	int FP, FN, TP, TN;
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
		FP = 0;
		FN = 0;
		TP = 0;
		TN = 0;

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
			found = false;
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
			found = false;
		}

		R = (double)TP / (double)(TP + FN);

		P = (double)TP / (double)(TP + FP);

		if (_isnan(P))
			P = 0.0;
		if (_isnan(R))
			R = 0.0;
		F = (double)(1 + 1)*((P*R) / ((1 * P) + R));
		if (_isnan(F))
			F = 0.0;
	}
};