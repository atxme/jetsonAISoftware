import socket
import threading

class SocketServer:
    def __init__(self, ip: str, port: int = 8888):
        self.client_socket = None
        self.ip = ip
        self.port = port
        self.buff_size = 1024
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.ip, self.port))

    def listen(self):
        self.socket.listen(5)
        self.client_socket, client_addr = self.socket.accept()
        print(f"Connection established with {client_addr}")

    def recv(self):
        cpt = 0
        enr = 0
        with self.client_socket:
            while True:
                # Réception des données
                data = self.client_socket.recv(self.buff_size)
                if data:
                    cpt = 0
                    try:
                        if data.decode().split(":")[0] == "size":
                            self.buff_size = int(data.decode().split(":")[1])
                            self.client_socket.send(b"ok")
                    except:
                        pass
                        # Écriture dans un fichier : MODIFIER LE PATH
                        with(open(f"C:/Users/mariu/Desktop/test{enr}.png", "ab")) as f:
                            f.write(data)
                        enr += 1
                        self.buff_size = 1024
                if cpt == 1000:
                    break
                cpt += 1

    def start(self):
        self.listen()
        self.recv()

    def __sendControlData(self, data):
        self.client_socket.send(data.encode('utf-8'))

    def sendControlData(self, data):
        thread = threading.Thread(target=self.__sendControlData, args=(data,))
        thread.start()

