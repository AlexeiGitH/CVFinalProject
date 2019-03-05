#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

void help(char** argv) {
	std::cout << "\n"
		<< "2.2: Like 2.1, but 'using namespace cv: \n"
		<< argv[0] << " <path/image>\n"
		<< "For example:\n"
		<< argv[0] << " ../fruits.jpg\n"
		<< std::endl;
}

VideoCapture vc;

///This method turns an image into a grayscale image 
void makeGray(cv::Mat& src, cv::Mat& dst);

///a method to create a binary image from an input image
cv::Mat createBinaryImage(cv::Mat& src, int threshold, double maxVal);
void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal = 255);

template <class T>
void askForInput(string& message, T& value) {
	cout << message;
	cin >> value;
}


int main(int argc, char** argv) {

	std::string groupName = "Group 4";
	cv::VideoCapture cap = cv::VideoCapture("fixedzone.mp4");

	if (!cap.isOpened()) { // check if we succeeded
		std::cerr << "Couldn't open capture." << std::endl;
		return -1;
	}

	cv::Mat videoFrame;
	int totalFrames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT); //get total frame count
	int frameNumber = 1;
	int brighter = 100;
	int threshold = 175;
	int canyThreshold = 175;
	int lineThreshold = 175;
	double maxVal = 255;
	double theta = CV_PI/ 45;
	for (;;) {		
		
		cap >> videoFrame;
		//videoFrame += cv::Scalar(brighter, brighter, brighter); //make the copy brighter
		cv::Mat frame; //scaled and processed frame
		cv::resize(videoFrame, frame, Size(), 1, 1);
		//cv::imshow(groupName, frame);
		string windowName = groupName;

		cv::Mat gray, binary, binaryBlured;
		makeGray(frame, gray);
		
		//BINARY FROM GRAY
		createBinaryImage(gray, binary, threshold);
		imshow(windowName + ". Binary", binary);

		//BLUR BINARY
		cv::medianBlur(binary, binaryBlured, 19);
		
		//DILATE BLURED BINARY
		cv::Mat dilated;  // Result output
		cv::dilate(binaryBlured, dilated, Mat(), Point(-1, -1), 2, 1, 1);

		//imshow(windowName + ". Dialted", dilated);

		Mat canny;
		Canny(gray, canny, canyThreshold, maxVal, 3);
		//imshow(windowName + ". Canny", canny);

		//MASK 
		Mat cannyMasked;
		canny.copyTo(cannyMasked, dilated);
		//imshow(windowName + ". Canny_Masked", cannyMasked);

		cv::Mat cannyMaskedDilated;  // Result output
		cv::dilate(cannyMasked, cannyMaskedDilated, Mat(), Point(-1, -1), 1, 1, 1);
		imshow(windowName + ". Canny_Masked_Dilated", cannyMaskedDilated);

		vector<Vec2f> lines;
		cv::HoughLines(cannyMaskedDilated, lines, 7, theta, lineThreshold);

		for (size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1]; Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			//cout << "pt1: " << pt1.x << ", " << pt1.y << endl;
			//cout << "pt2: " << pt2.x << ", " << pt2.y << endl;
			line(frame, pt1, pt2, Scalar(0, 225, 255), 1, CV_AA);
		}

		///////////////////////////////////////////////////////////Human detection

		/// Set up the pedestrian detector --> let us take the default one
		HOGDescriptor hog;
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

		/// Set up tracking vector
		vector<Point> track;

		/// run the detector with default parameters. to get a higher hit-rate
		/// (and more false alarms, respectively), decrease the hitThreshold and
		/// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).

		///image, vector of rectangles, hit threshold, win stride, padding, scale, group th
		Mat img = videoFrame.clone();
		resize(img, img, Size(img.cols * 2, img.rows * 2));

		vector<Rect> found;
		vector<double> weights;

		hog.detectMultiScale(img, found, weights);

		/// draw detections and store location
		for (size_t i = 0; i < found.size(); i++)
		{
			Rect r = found[i];
			rectangle(img, found[i], cv::Scalar(0, 0, 255), 3);
			stringstream temp;
			temp << weights[i];
			putText(img, temp.str(), Point(found[i].x, found[i].y + 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
			track.push_back(Point(found[i].x + found[i].width / 2, found[i].y + found[i].height / 2));
		}

		/// plot the track so far
		for (size_t i = 1; i < track.size(); i++) {
			line(frame, track[i - 1], track[i], Scalar(255, 255, 0), 2);
		}

		/// Show
		//imshow("detected person", img);
		//waitKey(1);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		cv::imshow(groupName, frame);
		//Check if Ran out of film
		frameNumber++;
		if (frameNumber > totalFrames) {
			frameNumber = 1;
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); // reset video frame to 0
		}

		char option = waitKey(40);
		if (option >= 0)
		{
			if (option == 67) //Capital C
			{
				canyThreshold += 2;
				cout << "canyThreshold: " << canyThreshold << endl;
			}if (option == 99) //Lowercase C
			{
				canyThreshold -= 2;
				cout << "canyThreshold: " << canyThreshold << endl;
			}
			if (option == 76) //Capital L
			{
				lineThreshold += 2;
				cout << "lineThreshold: " << lineThreshold << endl;
			}if (option == 108) //Lowercase L
			{
				lineThreshold -= 2;
				cout << "lineThreshold: " << lineThreshold << endl;
			}
			if (option == 66) //Capital B
			{
				brighter += 5;
				cout << "Brightness: " << brighter << endl;
			}if (option == 98) //Lowercase b
			{
				brighter -= 5;
				cout << "Brightness: " << brighter << endl;
			}
			if (option == 84) //Capital T
			{
				threshold += 5;
				cout << "threshold: " << threshold << endl;
			}if (option == 116) //Lowercase t
			{
				threshold -= 5;
				cout << "threshold: " << threshold << endl;
			}
			if (option == 77) //Capital M
			{
				maxVal += 5;
				cout << "Max val: " << maxVal << endl;
			}
			if (option == 109) //Lowercase m
			{
				maxVal -= 5;
				cout << "MaxVal: " << maxVal << endl;
			}
			if (option == 72) //Upper H
			{
				theta += 0.05;
				cout << "theta: " << theta << endl;
			}
			if (option == 104) //Lowercase h
			{
				theta -= 0.05;
				cout << "theta: " << theta << endl;
			}
			if (option == 13)
				break;
		}
			
	}
	cap.release();
	
	destroyAllWindows();
	return 0;
}


void makeGray(cv::Mat& src, cv::Mat& dst) {
	cv::cvtColor(src,dst,CV_RGB2GRAY);
}

cv::Mat createBinaryImage(cv::Mat& src, int threshold, double maxVal){
	Mat dst;
	cv::threshold(src,dst,threshold, maxVal, THRESH_BINARY);
	return dst;
}

void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal) {
	cv::threshold(src, dst, threshold, maxVal, THRESH_BINARY);
}

