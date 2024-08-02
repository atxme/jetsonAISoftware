import cv2 as cv 
import numpy as np
import os
import sys 
from threading import Thread


class VideoFlux:


    def initCamera(self):
        for i in range(10):
            self.ret, self.frame = self.cap.read()
            if self.ret:
                self.capIdx = i
                break
            else:
                None

        if (self.cap.isOpened != True):
            print("Error: Camera not found")
            sys.exit(0)

    def getFrame(self):




    def __init__(self) -> None:
        self.cap = cv.VideoCapture(0)
        self.capIdx = 0
        self.cap.set(3, 640)
        self.cap.set(4, 480)
        self.cap.set(10, 100)
        self.frame = None
        self.ret = None
        self.isOpened = None
        self.frame = None
