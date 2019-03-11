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

	double fps = cap.get(CV_CAP_PROP_FPS);
	// Default resolution of the frame is obtained.The default resolution is system dependent. 
	int inputVideoFrameWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int inputVideoFrameHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	//get total frame count
	int totalFrames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT);
	int codecType = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));

	string outputVideoName = (string)argv[1] + "-out.avi";
	Size size = Size(inputVideoFrameWidth*0.2, inputVideoFrameHeight*0.2);

	VideoWriter outputVideo;                                        // Open the output
	outputVideo.open(outputVideoName, -1, cap.get(CV_CAP_PROP_FPS), size, true);

	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: " << argv[1] << endl;
	}

	//VideoWriter videoResult("Fragment.out.mp4", CV_FOURCC('X', '2', '6', '4'), fps, Size(frame_width, frame_height));

	int brighter = -20;
	int binaryImageThreshold = 130;
	int cannyThreshold1 = 230;
	int cannyThreshold2 = 250;
	int lineThreshold = 175;
	double maxVal = 255;
	double theta = CV_PI / 1.5;
	double thetaLeftDiagonal = CV_PI / 4.72;
	double rho = 4.5;

	int frameNumber = 1;
	cv::Mat videoFrame;
	for (;;) {

		if (frameNumber % 5) {
			cout << "Frame number is " << frameNumber << endl;
		}
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



		Size frameSize = frame.size();
		int frameWidth = frameSize.width;
		int frameHeight = frameSize.height;

		// 5) Line detection
		if (true) {
			vector<Vec2f> lines;
			cv::HoughLines(closed, lines, rho, theta, lineThreshold);


			for (size_t i = 0; i < lines.size(); i++)
			{
				float rho = lines[i][0], theta = lines[i][1];

				Point pt1, pt2;
				double a = cos(theta), b = sin(theta);
				double x0 = a * rho, y0 = b * rho;
				//cout << "a (cos(theta)) = " << a << "; b (sin(theta)) = " << b << endl;
				//cout << "x0 = " << x0 << "; y0  = " << y0 << endl;
				pt1.x = cvRound(x0 + frameWidth * (-b));
				pt1.y = cvRound(y0 + inputVideoFrameHeight * (a));
				pt2.x = cvRound(x0 - frameWidth * (-b));
				pt2.y = cvRound(y0 - inputVideoFrameHeight * (a));
				//cout << "pt1: " << pt1.x << ", " << pt1.y << endl;
				//cout << "pt2: " << pt2.x << ", " << pt2.y << endl;
				line(frame, pt1, pt2, Scalar(0, 225, 255), 1, CV_AA);
			}
		}
		else {
			vector<Vec4i> lines;
			HoughLinesP(closed, lines, rho, theta, lineThreshold, 30, 10);
			for (size_t i = 0; i < lines.size(); i++)
			{
				Vec4i l = lines[i];
				line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, LINE_AA);
			}

		}


		cv::imshow(groupName, frame);

		// Write the frame into the file 'outcpp.avi'
		//outputVideo << frame;

		//Check if Ran out of film
		frameNumber++;
		if (frameNumber > totalFrames) {
			//break;
			frameNumber = 1;
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); // reset video frame to 0
		}

		char option = waitKey(fps);
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
	outputVideo.release();

	destroyAllWindows();
	return 0;
}


void makeGray(cv::Mat& src, cv::Mat& dst) {
	cv::cvtColor(src, dst, CV_RGB2GRAY);
}

cv::Mat createBinaryImage(cv::Mat& src, int threshold, double maxVal) {
	Mat dst;
	cv::threshold(src, dst, threshold, maxVal, THRESH_BINARY);
	return dst;
}

void createBinaryImage(cv::Mat& src, cv::Mat& dst, int threshold, double maxVal) {
	cv::threshold(src, dst, threshold, maxVal, THRESH_BINARY);
}

