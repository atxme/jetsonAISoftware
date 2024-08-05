from neuralEngine import NeuralEngine
import cv2 as cv 
import numpy as np
import time
import os
import sys
import threading
import pycuda.driver as cuda
import pycuda.autoinit
from queue import Queue


class ImageProcessing:

    def __init__(self, neural_engine: NeuralEngine, image_queue: Queue):
        self.neural_engine = neural_engine
        self.image_queue = image_queue

    def run(self):
        while True:
            if not self.image_queue.empty():
                img = self.image_queue.get()
                result = self.neural_engine.infer(img)
                # Traitement des résultats
                print("Inference result:", result)
                cv.imshow("Processed Video", img)
                if cv.waitKey(1) & 0xFF == ord('q'):
                    break