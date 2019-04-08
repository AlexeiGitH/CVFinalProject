import numpy as np
import cv2
import argparse
from imutils.paths import list_images
from selectors import BoxSelector

#parse arguments
ap = argparse.ArgumentParser()
ap.add_argument("-d","--dataset",required=True,help="path to images dataset...")
ap.add_argument("-a","--annotations",required=True,help="path to save annotations...")
ap.add_argument("-i","--images",required=True,help="path to save images")
args = vars(ap.parse_args())

#annotations and image paths
annotations = []
imPaths = []
rects = []

#loop through each image and collect annotations
for imagePath in list_images(args["dataset"]):

    #load image and create a BoxSelector instance
    image = cv2.imread(imagePath)
    bs = BoxSelector(image,"Image")
    #cv2.imshow("Image",image)
    #cv2.namedWindow("Image")
    #cv2.waitKey(0)
    #order the points suitable for the Object detector
    annotations.append(bs.rects)
    imPaths.append(imagePath)

#save annotations and image paths to disk
#print ("rects", annotations)
annotations = np.array(annotations)
imPaths = np.array(imPaths,dtype="unicode")
np.save(args["annotations"],annotations)
np.save(args["images"],imPaths)
