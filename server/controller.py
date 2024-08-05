import os 
import sys 
import threading
import numpy as np 
import pygame
from pygame.locals import *
import time

class CxController :
    
    def __init__(self) -> None:
        pygame.init()
        pygame.joystick.init()
        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        self.axis = [0, 0, 0, 0]

    def listControllerAvailable():
        joystick_count = pygame.joystick.get_count()
        print(f"Nombre de contrôleurs détectés : {joystick_count}")
        for i in range(joystick_count):
            joystick = pygame.joystick.Joystick(i)
            joystick.init()
            print(f"Contrôleur {i}:")
            print(f"  Nom: {joystick.get_name()}")
            print(f"  Nombre d'axes: {joystick.get_numaxes()}")
            print(f"  Nombre de boutons: {joystick.get_numbuttons()}")
            print(f"  Nombre de chapeaux: {joystick.get_numhats()}")