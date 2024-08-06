from webRtcManagement import VideoFrameTrack, WebRTCClient
from videoFlux import VideoFlux
from network import SocketClient
from nvidia_racecar import NvidiaRacecar

def initMessage():
    print("Nvidia Jetson AI SOFTWARE")
    print("Version 1.0")
    print("Author: Christophe. B (2024)")
    print("Date: 2024-07-01")
    print("Public key of the binary:")
    with open("kPub.pem", "r") as file:
        lines = file.readlines()
        key_lines = [line.strip() for line in lines if "BEGIN PUBLIC KEY" not in line and "END PUBLIC KEY" not in line]
        public_key = ''.join(key_lines)
    print(public_key)
    print("\n")

def main():
    initMessage()

    # Create the Nvidia Racecar object
    raceCar = NvidiaRacecar()
    
    # Create the socket client
    socket = SocketClient(raceCar, "10.82.225.26", 8888)
    socket.run()

    # Create the video flux object
    videoFlux = VideoFlux()

    # Create the WebRTC client
    webrtc = WebRTCClient("http://localhost:8888", videoFlux)
    webrtc.start()

    videoFlux.run()

if __name__ == "__main__":
    main()