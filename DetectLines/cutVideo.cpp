#pragma once
#include "cutVideo.h"


bool cutVideo::cut5Seconds(string filename, cv::VideoCapture cap) {

	string name = filename.substr(0, filename.find_last_of("."));
	
	cv::Mat videoFrame;
	int totalFrames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT); //get total frame count
	int fps = (int)cap.get(CV_CAP_PROP_FPS); //get fps
	int frameNumber = rand() % (totalFrames - (fps/5)) + 1; //random frame from 1 to 5 seconds (60*5 frames) before the end
	cap.set(CV_CAP_PROP_POS_FRAMES, frameNumber); //set beginning frame to randomly picked frame

	int frame_width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH); //get original video height/width
	int frame_height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	VideoWriter video(name+"-short.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, Size(frame_width, frame_height), true); //begin capture (filename, decoder code, fps, size, bw/colour)

	for (int i = 0; i < (fps*5); i++) { //5 seconds, same fps
		Mat frame;
		cap >> frame;
		video.write(frame);
		frameNumber++;
	}

	return true;
}