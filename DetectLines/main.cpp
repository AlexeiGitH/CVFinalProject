#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib" )

using namespace cv;
using namespace std;

void help(char** argv) {
	std::cout << "\n"
		<< "Pass the name of the video file as an argument: \n"
		<< argv[0] << " <path/video>\n"
		<< "For example:\n"
		<< argv[0] << " ../video1.mp4\n"
		<< std::endl;
}

VideoCapture vc;

///This method turns an image into a grayscale image 
void makeGray(cv::Mat& src, cv::Mat& dst);

///a method to create a binary image from an input image
cv::Mat createBinaryImage(cv::Mat& src, int threshold, double binaryImageMaxVal);
void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal = 255);

template <class T>
void askForInput(string& message, T& value) {
	cout << message;
	cin >> value;
}

int main(int argc, char** argv) {

	if (argc < 1) {
		help(argv);
		return 0;
	}

	std::string groupName = "Group 4";
	cv::VideoCapture cap = cv::VideoCapture(argv[1]);

	if (!cap.isOpened()) { // check if we succeeded
		std::cerr << "Couldn't open video capture." << std::endl;
		return -1;
	}

	cv::Mat videoFrame;
	int totalFrames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT); //get total frame count
	int frameNumber = 1;


	int brighter = -20;
	int binaryImageThreshold = 130;
	int cannyThreshold1 = 230;
	int cannyThreshold2 = 250;
	int lineThreshold = 175;
	double maxVal = 255;
	double theta = CV_PI/1.5;
	double thetaLeftDiagonal = CV_PI / 4.72;
	double rho = 4.5;

	for (;;) {		
		
		cap >> videoFrame;
		videoFrame += cv::Scalar(brighter, brighter, brighter); //make the copy brighter
		cv::Mat frame; //scaled and processed frame
		cv::resize(videoFrame, frame, Size(), 0.2, 0.2);
		//cv::imshow(groupName, frame);
		string windowName = groupName;

		cv::Mat gray, binary, binaryBlured;
		makeGray(frame, gray);

		//1) Prepare an image to use as a mask
		//BINARY FROM GRAY
		createBinaryImage(gray, binary, binaryImageThreshold);
		imshow(windowName + ". Binary", binary);

		//BLUR BINARY
		cv::medianBlur(binary, binaryBlured, 19);
		
		//DILATE BLURED BINARY
		cv::Mat dilated;  // Result output
		cv::dilate(binaryBlured, dilated, Mat(), Point(-1, -1), 2, 1, 1);

		//Show Dialted
		imshow(windowName + ". Dialted", dilated);

		// 2) Detect edges using Canny alg.
		//Canny edge detection
		Mat canny;
		Canny(gray, canny, cannyThreshold1, cannyThreshold2, 3);
		imshow(windowName + ". Canny", canny);

		// 3) Mask an image with edges using an image created on step 1)
		Mat cannyMasked;
		canny.copyTo(cannyMasked, dilated);
		imshow(windowName + ". Canny_Masked", cannyMasked);

		// 4) Dilate to remove noise
		cv::Mat cannyMaskedDilated;  // Result output
		cv::dilate(cannyMasked, cannyMaskedDilated, Mat(), Point(-1, -1), 1, 1, 1);
		imshow(windowName + ". Canny_Masked_Dilated", cannyMaskedDilated);

		//	Apply morphological closing to the binary image using a 3x3 square kernel
		int kernelSize = 3; //kernel size
		cv::Mat elem = 255 * cv::Mat::ones(kernelSize, kernelSize, CV_8UC1); // 3x3 kernel matrix filled with 255
		cv::Mat closed;
		cv::morphologyEx(cannyMasked, closed, MORPH_CLOSE, elem);

		cv::namedWindow(groupName + ". Closed", cv::WINDOW_AUTOSIZE);
		cv::imshow(groupName + ". Closed", closed);


		// 5) Line detection
		vector<Vec2f> lines;
		cv::HoughLines(closed, lines, rho, theta, lineThreshold);

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

		cv::imshow(groupName, frame);
		//Check if Ran out of film
		frameNumber++;
		if (frameNumber > totalFrames) {
			frameNumber = 1;
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); // reset video frame to 0
		}

		// Apply morphological opening to the binary image using a 3x3 square kernel (all set to 255). 

		cv::Mat opened;
	
		cv::morphologyEx(cannyMasked, opened, MORPH_OPEN, elem); //aply opening

		cv::namedWindow(groupName + ". Opened", cv::WINDOW_AUTOSIZE);
		cv::imshow(groupName + ". Opened", opened);


		

		char option = waitKey(40);
		if (option >= 0)
		{
			//CANNY THRESHHOLD KEYS
			if (option == 67) //Capital C
			{
				cannyThreshold1 += 2;
				cout << "canyThreshold1: " << cannyThreshold1 << endl;
			}if (option == 99) //Lowercase C
			{
				cannyThreshold1 -= 2;
				cout << "canyThreshold1: " << cannyThreshold1 << endl;
			}
			if (option == 86) //Capital V
			{
				cannyThreshold2 += 2;
				cout << "cannyThreshold2: " << cannyThreshold2 << endl;
			}if (option == 118) //Lowercase V
			{
				cannyThreshold2 -= 2;
				cout << "cannyThreshold2: " << cannyThreshold2 << endl;
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
			//IMAGE BRIGTHNESS KEYS
			if (option == 66) //Capital B
			{
				brighter += 5;
				cout << "Brightness: " << brighter << endl;
			}if (option == 98) //Lowercase b
			{
				brighter -= 5;
				cout << "Brightness: " << brighter << endl;
			}

			//binaryImageThreshold
			if (option == 84) //Capital T
			{
				binaryImageThreshold += 5;
				cout << "threshold: " << binaryImageThreshold << endl;
			}if (option == 116) //Lowercase t
			{
				binaryImageThreshold -= 5;
				cout << "threshold: " << binaryImageThreshold << endl;
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

			//LINES DETECTION KEYS

			if (option == 72) //Upper H
			{
				theta += 0.01;
				cout << "theta: " << theta << endl;
			}
			if (option == 104) //Lowercase h
			{
				theta -= 0.01;
				cout << "theta: " << theta << endl;
			}
			if (option == 82) //Upper H
			{
				rho += 0.5;
				cout << "rho: " << rho << endl;
			}
			if (option == 114) //Lowercase h
			{
				rho -= 0.5;
				cout << "rho: " << rho << endl;
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

