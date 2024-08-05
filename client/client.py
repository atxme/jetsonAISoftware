import socket
from threading import Thread

class SocketClient:
    def __init__(self, server_ip: str, server_port: int = 8888):
        self.server_ip = server_ip
        self.server_port = server_port
        self.buffersize = 1024

        # Initialiser le socket pour la réception
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.initConnection()

        # Lancer le thread de réception des commandes
        self.receive_thread = Thread(target=self.receiveControlData, daemon=True)
        self.receive_thread.start()

    def initConnection(self):
        # Se connecter au serveur
        try:
            self.socket.connect((self.server_ip, self.server_port))
            print(f"Connected to server at {self.server_ip}:{self.server_port}")
        except socket.error as e:
            print(f"Could not connect to server: {e}")

    def receiveControlData(self):
        while True:
            try:
                data = self.socket.recv(self.buffersize)
                if data:
                    command = data.decode('utf-8')
                    print(f"Received command: {command}")
                    self.process_command(command)
            except socket.error as e:
                print(f"Error receiving data: {e}")
                break

    def process_command(self, command):
        # Traitez la commande ici
        print(f"Processing command: {command}")
        # Ajoutez ici le code pour exécuter les commandes reçues