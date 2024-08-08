import cv2 as cv
import numpy as np
import sys
from threading import Thread, Event
from queue import Queue
import subprocess

class VideoFlux:
    def __init__(self, serverIp: str, serverPort: int = 8888) -> None:
        self.capIdx = 0
        self.frame = None
        self.ret = None
        self.isOpened = False
        self.queue = Queue()
        self.pipelineProcess = None
        self.stop_event = Event()
        self.serverIp = serverIp
        self.serverPort = serverPort

    def initCamera(self):
        # Attempt to open camera
        for i in range(10):
            self.cap = cv.VideoCapture(i)
            if self.cap.isOpened():
                print(f"Camera found at index {i}")
                self.capIdx = i
                self.isOpened = True
                break
            self.cap.release()  # Release the camera if not opened

        if not self.isOpened:
            print("Error: Camera not found")
            sys.exit(1)
        
        # Set camera properties
        self.cap.set(cv.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
        self.cap.set(cv.CAP_PROP_FPS, 30)

    def initPipelineFlux(self):
        # Construct the GStreamer command with dynamic IP and port
        command = [
            'gst-launch-1.0',
            'appsrc', 'caps=video/x-raw,format=BGR,width=640,height=480,framerate=30/1',
            '!', 'videoconvert',
            '!', 'x264enc',
            '!', 'rtph264pay',
            '!', f'udpsink host={self.serverIp} port={self.serverPort}'
        ]
        
        self.pipelineProcess = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def captureFrame(self):
        while not self.stop_event.is_set():
            self.ret, self.frame = self.cap.read()
            if self.ret:
                self.queue.put(self.frame)
            else:
                self.stop_event.set()
            cv.waitKey(1)
        self.releaseCamera()

    def releaseCamera(self):
        if self.cap.isOpened():
            self.cap.release()
        cv.destroyAllWindows()

    def run(self):
        self.initCamera()
        self.initPipelineFlux()
        capture_thread = Thread(target=self.captureFrame)
        capture_thread.start()
        
        try:
            while not self.stop_event.is_set():
                if not self.queue.empty():
                    frame = self.queue.get()
                    self.pipelineProcess.stdin.write(frame.tobytes())
                    self.pipelineProcess.stdin.flush()
                    self.queue.task_done()
                    cv.imshow("Video", frame)
                cv.waitKey(1)
        finally:
            self.stop_event.set()
            capture_thread.join()
            self.pipelineProcess.stdin.close()
            self.pipelineProcess.wait()
            self.releaseCamera()


