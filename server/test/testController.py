import os 
import sys
from network import SocketServer
from controller import CxController




def test():
    os.system("hostname -I > ip.txt")
    with open("ip.txt", "r") as file:
        ip = file.read().strip()
    os.remove("ip.txt")

    # Create and start socket server
    socket = SocketServer(ip, 8888)  # Open a socket on local IP and port 8888
    CxController(socket).runSelfTest()

test()