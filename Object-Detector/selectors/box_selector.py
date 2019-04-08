import numpy as np
import cv2
import dlib

class BoxSelector(object):
    def __init__(self, image, window_name,color=(0,0,255)):
        #store image and an original copy
        self.image = image
        self.tmp = image.copy()

        #capture start and end point co-ordinates
        self.start = None
        self.end = None

		#all captured boxes
        self.boxes = []

        #flag to indicate tracking
        self.track = False
        self.color = color
        self.window_name = window_name

        #hook callback to the named window
        cv2.namedWindow(self.window_name)
        cv2.setMouseCallback(self.window_name,self.mouseCallBack)

        while(True):
			self.tmp = self.image.copy()
			if self.track == True:
				if not self.start==self.end:
	                #draw rectangle on the image
					cv2.rectangle(self.tmp, self.start, self.end, self.color, 2)

			cv2.imshow(self.window_name, self.tmp)
			key=cv2.waitKey(15)
			if key == 27 or key == 32:
				break

    def mouseCallBack(self, event, x, y, flags, params):
        #start tracking if left-button-clicked down
        if event==cv2.EVENT_LBUTTONDOWN:
            self.start = (x,y)
            self.end = (x,y)
            self.track = True

        #capture/end tracking while mouse-move or left-button-click released
        elif self.track and (event==cv2.EVENT_MOUSEMOVE or event==cv2.EVENT_LBUTTONUP):
            self.end = (x,y)
            if not self.start==self.end:
                if event==cv2.EVENT_LBUTTONUP:
                    self.end = (x,y)
                    self.track=False

        if event==cv2.EVENT_LBUTTONUP:
            self.track=False
            cv2.rectangle(self.image, self.start, self.end, self.color, 2)
            pts = np.array([self.start,self.end])
            s = np.sum(pts, axis=1)
            (x,y) = pts[np.argmin(s)]
            (xb,yb) = pts[np.argmax(s)]
            rect=dlib.rectangle(left=long(x),top=long(y),right=long(xb),bottom=long(yb))
            self.boxes.append(rect)
            self.start = None
            self.end = None

    @property
    def rects(self):
        return self.boxes

    @property
    def roiPts(self):
        if self.start and self.end:
            pts = np.array([self.start,self.end])
            #print ("pts: ", pts)
            s = np.sum(pts, axis=1)
            #print ("s: ", s, "np.argmin(s) = ", np.argmin(s))
            (x,y) = pts[np.argmin(s)]
            #print ("(x,y): ", (x,y), "x: ", x)
            (xb,yb) = pts[np.argmax(s)]
            return [(x,y),(xb,yb)]
        else:
            return []
