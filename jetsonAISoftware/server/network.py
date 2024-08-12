import socket
import threading

class SocketServer:
    def __init__(self, ip: str, port: int = 8888):
        self.ip = ip
        self.port = port
        self.buff_size = 1024
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.ip, self.port))
        self.client_socket = None

    def listen(self):
        self.socket.listen(5)
        print(f"Listening on {self.ip}:{self.port}")
        self.client_socket, client_addr = self.socket.accept()
        print(f"Connection established with {client_addr}")

    def sendControlData(self, data):
        try:
            self.client_socket.send(data.encode('utf-8'))
        except socket.error as e:
            print(f"Failed to send data: {e}")


