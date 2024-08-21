from ctypes import cdll
from racecar import Racecar
from nvidia_racecar import NvidiaRacecar
import os
from threading import Thread
from network import SocketClient

def main():
    l_sPath = os.path.dirname(os.path.realpath(__file__))
    print(f"Current path: {l_sPath}")

    so_path = os.path.join(l_sPath, "streamer.so")
    print(f"Loading library from: {so_path}")

    l_soLib = cdll.LoadLibrary(so_path)
    
    l_oRaceCar = Racecar()
    l_oNvidiaRaceCar = NvidiaRacecar(l_oRaceCar)

    l_oSocket = SocketClient(l_oNvidiaRaceCar, "10.82.251.197", 8888)
    l_oSocket.run()

    # start the streamer process
    l_oStreamer = Thread(target=l_soLib.main, args=())




main()
