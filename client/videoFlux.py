import cv2 as cv 
import numpy as np
import os
import sys 
from threading import Thread
from queue import Queue


class VideoFlux:

    def __init__(self) -> None:
        self.capIdx = 0
        self.frame = None
        self.ret = None
        self.isOpened = None
        self.frame = None
        self.queue = Queue()

    def initCamera(self):
        for i in range(10):
            self.cap = cv.VideoCapture(i)
            self.isOpened = self.cap.isOpened()
            if self.isOpened:
                print(f"Camera found at index {i}")
                self.capIdx = i
                break
            else:
                i+=1
        
        if (self.cap.isOpened() != True):
            print("Error: Camera not found")
            sys.exit(0)
        
        self.cap.set(cv.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
        self.cap.set(cv.CAP_PROP_FPS, 30)

        
    def captureFrame(self):
        while True:
            self.ret, self.frame = self.cap.read()
            self.queue.put(self.frame)
            if cv.waitKey(1) & 0xFF == ord('q'):
                break
        self.releaseCamera()

    def releaseCamera(self):
        self.cap.release()
        cv.destroyAllWindows()

    
    def run(self):
        self.initCamera()
        Thread(target=self.captureFrame).start()




        



    
