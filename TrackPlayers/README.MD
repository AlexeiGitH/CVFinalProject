#Program to detect players

Structure:

## HogDetector.cpp: 
    Wrapper class to easily use HOG detector
    hog: instance of a HogDetector with the specified parameters

    detect(): runs the hog detector and returns a vector of found bounding rectangles

    adjustRect(): Slightly adjusts the returned rectangles to make them cleaner

    descrition(): Returns a string of all the detector parameters

## Detector.hpp: 
    The main class that has methods to prepare images for the processing.
    Has HogDetector class instance with HOG_People_Defualt model 
    
    MakeMask() - that automatically detects the rink area to mask the image and remove the crowd are.
        Applies morph opening to remove small details, Blur the image, Dilate to remove some big chunks, Erode to return to get back lost area of the rink, Use bitwise flip to get a mask. Comments are in Detector.hpp.

    PrepareForProcessing() makes players bodies well defined on rink by taking a difference between the original and blurred+dilated image. (pass option 2 to the method)
    
    Detect() - detects player on image without pre-processing the image (detect on the image you pass, you can prepare your own image as you like)
    
    DetectWithPreprocessing() - detects player on image preparing the image first (mask + contours)

## Main.cpp: Main program to run player detection, MUST PASS IN IMAGE FILENAME
    main(): reads passed in image, determines new filenames for output images, and calls all the functions

    DrawDetected(): draws detected rectangles on the source image

    save(): saves passed in image to current folder


MakeMask(): Add contrast, Convert to grayscale, Threshhold gray image, Use big kernel size to remove players and noise from the rink


MarkDetectedPlayer(): Draws bounding rectangles and circles on the source image




