import gi
import threading
import socket
import queue
import time
from gi.repository import Gst, Gtk

gi.require_version('Gst', '1.0')

# Initialize GStreamer
Gst.init(None)

class VideoReceiver(threading.Thread):
    def __init__(self, port: int = 50000):
        super().__init__()
        self.port = port
        self.pipeline = None
        self.running = threading.Event()
        self.running.set()
        self.pipeline_thread = threading.Thread(target=self._pipeline_loop)
        self.pipeline_thread.start()

    def run(self):
        # Run the GTK main loop in the main thread
        Gtk.main()

    def _pipeline_loop(self):
        self.init_pipeline()
        # Block the thread until the event is set to stop
        while self.running.is_set():
            time.sleep(1)  # Simple keep-alive for the pipeline

    def init_pipeline(self):
        self.pipeline = Gst.parse_launch(
            f"udpsrc port={self.port} ! application/x-rtp,encoding-name=H264,payload=96 ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink"
        )
        self.pipeline.set_state(Gst.State.PLAYING)
        print("GStreamer pipeline initialized and playing")

    def stop(self):
        self.running.clear()
        self.pipeline.set_state(Gst.State.NULL)
        self.pipeline_thread.join()
        Gtk.main_quit()