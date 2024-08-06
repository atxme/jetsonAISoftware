import socket
import threading
from nvidia_racecar import NvidiaRacecar

class SocketClient:
    
    def __init__(self, nvidiaRaceCar: NvidiaRacecar, serverIp: str, serverPort: int = 8888) -> None:
        self.serverIp = serverIp
        self.serverPort = serverPort
        self.bufferSize = 1024

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.socket.connect((self.serverIp, self.serverPort))
            print(f"Connected to server at {self.serverIp}:{self.serverPort}")
        except socket.error as e:
            print(f"Could not connect to server: {e}")

        self.raceCar = nvidiaRaceCar
        self.running = True

    def receiveControlData(self):
        while self.running:
            try:
                data = self.socket.recv(self.bufferSize)
                if data:
                    command = data.decode('utf-8')
                    self.processControlData(command)
            except socket.error as e:
                print(f"Error receiving data: {e}")
                self.running = False

    def processControlData(self, command: str) -> None:
        try:
            command_values = command.split(";")
            self.raceCar.throttle = float(command_values[0])
            self.raceCar.steering = float(command_values[1])
        except (IndexError, ValueError) as e:
            print(f"Error processing command: {e}")

    def run(self) -> None:
        self.receive_thread = threading.Thread(target=self.receiveControlData, daemon=True)
        self.receive_thread.start()

    def stop(self) -> None:
        self.running = False
        self.socket.close()
        if self.receive_thread.is_alive():
            self.receive_thread.join()