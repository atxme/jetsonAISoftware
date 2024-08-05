import pygame
from pygame.locals import *
import time

# Initialiser pygame pour la gestion des entrées
pygame.init()
pygame.joystick.init()

def list_controllers():
    # Nombre de contrôleurs connectés
    joystick_count = pygame.joystick.get_count()
    print(f"Nombre de contrôleurs détectés : {joystick_count}")

    # Itérer sur chaque contrôleur connecté
    for i in range(joystick_count):
        joystick = pygame.joystick.Joystick(i)
        joystick.init()  # Initialiser chaque joystick pour pouvoir l'interroger

        # Afficher des informations détaillées sur chaque contrôleur
        print(f"Contrôleur {i}:")
        print(f"  Nom: {joystick.get_name()}")
        print(f"  Nombre d'axes: {joystick.get_numaxes()}")
        print(f"  Nombre de boutons: {joystick.get_numbuttons()}")
        print(f"  Nombre de chapeaux: {joystick.get_numhats()}")

def main():
    print("Démarrage du test des contrôleurs...")
    list_controllers()

    try:
        # Boucle pour garder le script en écoute des entrées
        while True:
            time.sleep(0.1)  # Attendre un peu pour ne pas surcharger le processeur
            pygame.event.pump()  # Mise à jour des événements pygame

            # Itérer sur chaque contrôleur connecté
            for i in range(pygame.joystick.get_count()):
                joystick = pygame.joystick.Joystick(i)
                joystick.init()

                # Lire les axes
                axes = [joystick.get_axis(j) for j in range(joystick.get_numaxes())]
                print(f"Contrôleur {i} axes: {axes}")



    except KeyboardInterrupt:
        print("Fermeture du script.")
    finally:
        # Nettoyer en quittant
        pygame.quit()

if __name__ == "__main__":
    main()
