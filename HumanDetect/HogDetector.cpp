#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

enum HOGDetectorMode { Default, Daimler };
class HOGDetector
{
	HOGDetectorMode mode_;
	HOGDescriptor hog;
public:
	double hitThreshold;
	Size winStride;
	Size padding;
	double scale;

	//These parameters work well. Scale any other than 1.05 does not work as well as when the threshold is >< 0
	HOGDetector(HOGDetectorMode mode): hog(), hitThreshold(0), winStride(Size(8, 8)), padding(Size(32, 32)), scale(1.05)
	{
		mode_ = mode;
		switch (mode_)
		{
		case HOGDetectorMode::Default:
			hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
			break;
		case HOGDetectorMode::Daimler:
			//this one crashes, idk why
			hog.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
			break;
		default:
			break;
		}
		
	}

	vector<Rect> detect(InputArray img)
	{
		// Run the detector with default parameters. to get a higher hit-rate
		// (and more false alarms, respectively), decrease the hitThreshold and
		// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
		vector<Rect> found;
		hog.detectMultiScale(img, found, hitThreshold, winStride, padding, scale, 2, false);
		return found;
	}
	Rect adjustRect(Rect& r) const
	{
		// The HOG detector returns slightly larger rectangles than the real objects,
		// so we slightly shrink the rectangles to get a nicer output.
		Rect adjusted = r;
		adjusted.x += cvRound(adjusted.width*0.1);
		adjusted.width = cvRound(adjusted.width*0.8);
		adjusted.y += cvRound(adjusted.height*0.07);
		adjusted.height = cvRound(adjusted.height*0.8);
		return adjusted;
	}

	//Method to get all the parameters of this detector
	string description() {
		stringstream ss;
		string modeName = "Default";
		
		if (mode_ == HOGDetectorMode::Daimler) {
			modeName = "Daimler";
		}
		ss << modeName;
		ss << "; hitThreshold(" << to_string(hitThreshold) << ");";
		ss << "winStride(" << to_string(winStride.width) << "|" << to_string(winStride.height) << ");";
		ss << "padding(" << to_string(padding.width) << "|" << to_string(padding.height) << ");";
		ss << "scale(" << to_string(scale) << ")";
		return ss.str();	
	}
};