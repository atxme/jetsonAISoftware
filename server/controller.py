import os 
import sys 
import threading
import numpy as np 
import pygame
from pygame.locals import *
import time
from network import SocketServer

class CxController :
    
    def __init__(self, socket : SocketServer) -> None:
        pygame.init()
        pygame.joystick.init()
        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        self.axis = [0, 0]
        self.dead_zone = 0.1
        self.socket = socket

    def listControllerAvailable(self) -> None:
        joystick_count = pygame.joystick.get_count()
        print(f"Nombre de contrôleurs détectés : {joystick_count}")

        if (joystick_count == 0):
            print("Aucun contrôleur détecté.")
            sys.exit(0)
        
        for i in range(joystick_count):
            joystick = pygame.joystick.Joystick(i)
            joystick.init()
            print(f"Contrôleur {i}:")
            print(f"  Nom: {joystick.get_name()}")
            print(f"  Nombre d'axes: {joystick.get_numaxes()}")
            print(f"  Nombre de boutons: {joystick.get_numbuttons()}")
            print(f"  Nombre de chapeaux: {joystick.get_numhats()}")


    def getControllerAxis(self) -> list:

        pygame.event.pump()

        # Joystick gauche (vertical)
        vertical_axis_value = self.joystick.get_axis(1)  # Axe 1 pour le mouvement vertical

        # Appliquer la zone morte
        if abs(vertical_axis_value) < self.dead_zone:
            vertical_axis_value = 0
        else:
            vertical_axis_value = vertical_axis_value

        # Inverser la valeur pour que le haut soit +1 et le bas soit -1
        self.axis[0] = -vertical_axis_value

        # Joystick droit (horizontal)
        horizontal_axis_value = self.joystick.get_axis(2)  # Axe 2 pour le mouvement horizontal

        # Appliquer la zone morte
        if abs(horizontal_axis_value) < self.dead_zone:
            horizontal_axis_value = 0
        else:
            horizontal_axis_value = horizontal_axis_value

        # Map horizontal axis value: gauche (-1), droite (+1)
        self.axis[1] = horizontal_axis_value

        return self.axis
    
    def runSelfTest() -> None:

        pygame.init()
        pygame.joystick.init()
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        axis = [0, 0]
        dead_zone = 0.1

        try:
            while True:
                time.sleep(0.1)
                pygame.event.pump()
                vertical_axis_value = joystick.get_axis(1)
                if abs(vertical_axis_value) < dead_zone:
                    vertical_axis_value = 0
                else:
                    vertical_axis_value = vertical_axis_value
                axis[0] = -vertical_axis_value
                horizontal_axis_value = joystick.get_axis(2)
                if abs(horizontal_axis_value) < dead_zone:
                    horizontal_axis_value = 0
                else:
                    horizontal_axis_value = horizontal_axis_value
                axis[1] = horizontal_axis_value
                print(f"Axis: {axis}")
        except KeyboardInterrupt:
            print("Fermeture du script.")
        finally:
            pygame.quit()

        
    def run() -> None:

        try:
            while True:
                time.sleep(0.5)
                pygame.event.pump()
                for i in range(pygame.joystick.get_count()):
                    joystick = pygame.joystick.Joystick(i)
                    joystick.init()
                    axes = [joystick.get_axis(j) for j in range(joystick.get_numaxes())]
                    
        except KeyboardInterrupt:
            print("Fermeture du script.")
        finally:
            pygame.quit()



