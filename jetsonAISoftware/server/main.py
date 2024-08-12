from webRtcManagement import RtcSession
from neuralEngine import NeuralEngine
from controller import CxController
from network import SocketServer
from imageProcessing import ImageProcessing
import argparse
import os
import sys
import threading



def main():
    # Load socket server
    # Get local IP first
    os.system("hostname -I > ip.txt")
    with open("ip.txt", "r") as file:
        ip = file.read().strip()
    os.remove("ip.txt")

    # Create and start socket server
    socket = SocketServer(ip, 8888)  # Open a socket on local IP and port 8888

    # Initialize neural engine
    engine = NeuralEngine()
    engine.load_engine("model.engine")

    # Create and start controller thread
    controller = CxController(socket)
    controller.listControllerAvailable()  # Perform self-test
    controller_thread = threading.Thread(target=controller.run)

    # Create WebRTC session and start it
    webrtc = RtcSession(ip, engine)

    # Create and start image processing thread
    image_processor = ImageProcessing(engine, webrtc.image_queue)
    image_processor_thread = threading.Thread(target=image_processor.run)

    webrtc_thread = threading.Thread(target=webrtc.start)

    # Start threads
    controller_thread.start()
    webrtc_thread.start()
    image_processor_thread.start()

    # Wait for threads to finish
    controller_thread.join()
    webrtc_thread.join()
    image_processor_thread.join()


if __name__ == "__main__":
    main()