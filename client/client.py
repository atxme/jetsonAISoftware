import socket
from threading import Thread

class SocketClient:
    def __init__(self, RxIp: str, RxPort: int = 8888, TxIp: str = None, TxPort: int = 8888):
        # Initialiser les adresses IP et ports
        self.RxIp = RxIp
        self.RxPort = RxPort  
        self.TxIp = TxIp if TxIp is not None else RxIp
        self.TxPort = TxPort
        
        # Initialiser les sockets pour la réception et l'émission
        self.RxSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.TxSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        self.isConnected = False
        self.buffer = []
        self.buffersize = 1024
        self.ServerCommandSize = 16
        
        self.initConnection()
        self.receive_thread = Thread(target=self.receiveControlData, daemon=True)
        self.receive_thread.start()

    def initConnection(self):
        # Configuration de la connexion de réception
        try:
            self.RxSocket.bind((self.RxIp, self.RxPort))
            self.RxSocket.listen(1)
            print(f"Listening for incoming connections on {self.RxIp}:{self.RxPort}")
        except socket.error as e:
            print(f"Failed to bind Rx socket: {e}")

        # Configuration de la connexion d'émission (connexion à un serveur)
        try:
            self.TxSocket.connect((self.TxIp, self.TxPort))
            self.isConnected = True
            print(f"Connected to server at {self.TxIp}:{self.TxPort}")
        except socket.error as e:
            print(f"Could not connect to server: {e}")

    def __sendImage(self, image):
        with open(image, "rb") as f:
            file = f.read()
        self.TxSocket.send(f"size:{len(file)}".encode('utf-8'))
        while self.TxSocket.recv(1024).decode('utf-8') != "ok":
            continue
        self.TxSocket.send(file)

    def send(self, image):
        thread = Thread(target=self.__sendImage, args=(image,))
        thread.start()

    def receiveControlData(self):
        while True:
            try:
                conn, addr = self.RxSocket.accept()
                print(f"Connection established with {addr}")
                with conn:
                    while True:
                        data = conn.recv(self.buffersize)
                        if not data:
                            break
                        self.buffer.append(data.decode('utf-8'))

                        # Si la taille du buffer atteint la taille de la commande, traitez les données
                        if len(self.buffer) == self.ServerCommandSize:
                            print(f"Received complete command: {self.buffer}")
                            self.process_command(self.buffer)
                            self.buffer = []  # Réinitialiser le buffer pour la prochaine commande

            except socket.error as e:
                print(f"Error receiving data: {e}")
                break

    def process_command(self, command):
        # Update des paramètres de la voiture en fonction de la commande actuelle
        print(f"Processing command: {command}")
        # Ici vous pouvez ajouter le code pour mettre à jour les paramètres de la voiture

