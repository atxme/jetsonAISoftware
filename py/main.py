from ctypes import cdll
from racecar import Racecar
from nvidia_racecar import NvidiaRacecar
import os
from threading import Thread
from network import SocketClient

def main():
    l_sPath = os.path.dirname(os.path.realpath(__file__))
    print(f"Current path: {l_sPath}")

    so_path = os.path.join(l_sPath, "libstreamer.so")
    print(f"Loading library from: {so_path}")

    l_soStreamerLib = cdll.LoadLibrary(so_path)
    
    #l_oSocket = SocketClient(l_oNvidiaRaceCar, "10.82.251.197", 8888)
    #l_oSocket.run()

    # start the streamer process
    l_soStreamer = Thread(target=l_soStreamerLib.main, args=(), daemon=False)
    l_soStreamer.start()




main()



