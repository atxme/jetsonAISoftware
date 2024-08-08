import cv2 as cv
import gi
import numpy as np
from threading import Thread, Event
from queue import Queue
import sys

gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib

Gst.init(None)

class VideoFlux:
    def __init__(self, serverIp: str, serverPort: int = 8888) -> None:
        self.capIdx = 0
        self.frame = None
        self.ret = None
        self.isOpened = False
        self.serverIp = serverIp
        self.serverPort = serverPort
        self.pipeline = None
        self.appsrc = None
        self.stop_event = Event()
        self.queue = Queue()

    def initCamera(self):
        for i in range(10):
            self.cap = cv.VideoCapture(i)
            if self.cap.isOpened():
                print(f"Camera found at index {i}")
                self.capIdx = i
                self.isOpened = True
                break
            self.cap.release()
        if not self.isOpened:
            print("Error: Camera not found")
            sys.exit(1)
        print("Camera opened successfully")
        self.cap.set(cv.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv.CAP_PROP_FRAME_HEIGHT, 480)
        self.cap.set(cv.CAP_PROP_FPS, 30)

    def initPipelineFlux(self):
        self.pipeline = Gst.parse_launch(
            f"appsrc name=source is-live=true format=GST_FORMAT_TIME caps=video/x-raw,format=BGR,width=640,height=480,framerate=30/1 ! videoconvert ! x264enc ! rtph264pay ! udpsink host={self.serverIp} port={self.serverPort}"
        )
        self.appsrc = self.pipeline.get_by_name('source')
        self.pipeline.set_state(Gst.State.PLAYING)
        print("Pipeline initialized and playing")

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

    def pushFrameToPipeline(self, frame):
        if self.appsrc:
            # Convert the frame to the correct format
            buf = Gst.Buffer.new_allocate(None, frame.nbytes, None)
            buf.fill(0, frame.tobytes())
            buf.pts = Gst.CLOCK_TIME_NONE
            buf.dts = Gst.CLOCK_TIME_NONE
            buf.duration = Gst.CLOCK_TIME_NONE
            buf.offset = 0
            # Push the buffer into appsrc
            self.appsrc.emit('push-buffer', buf)

    def run(self):
        self.initCamera()
        self.initPipelineFlux()
        capture_thread = Thread(target=self.captureFrame)
        capture_thread.start()

        try:
            while not self.stop_event.is_set():
                if not self.queue.empty():
                    frame = self.queue.get()
                    # Ensure all interactions with GStreamer occur on the main thread
                    GLib.idle_add(self.pushFrameToPipeline, frame)
                    self.queue.task_done()
        finally:
            self.stop_event.set()
            capture_thread.join()
            self.pipeline.set_state(Gst.State.NULL)
            self.releaseCamera()

if __name__ == "__main__":
    video = VideoFlux(serverIp='10.82.249.10', serverPort=50000)  # Remplacez '192.168.1.100' par l'IP du serveur de réception
    video.run()
