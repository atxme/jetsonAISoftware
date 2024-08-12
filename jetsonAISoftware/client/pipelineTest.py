import cv2 as cv
import numpy as np

# Initialiser la capture vidéo
cap = cv.VideoCapture(0)

# Paramètres GStreamer pour l'envoi de vidéo avec caps
gstreamer_pipeline = (
    "appsrc caps=\"video/x-raw,format=BGRx,width=640,height=480,framerate=30/1\" ! videoconvert ! autovideosink"
)

# Initialiser l'écriture vidéo avec GStreamer
out = cv.VideoWriter(gstreamer_pipeline, cv.CAP_GSTREAMER, 0, 30, (640, 480), True)

if not out.isOpened():
    print("Erreur: Impossible d'ouvrir le pipeline GStreamer")
    cap.release()
    exit(1)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Envoyer l'image au pipeline GStreamer
    out.write(frame)

    # Afficher l'image capturée localement (facultatif)
    cv.imshow('Camera', frame)

    if cv.waitKey(1) & 0xFF == ord('q'):
        break

# Libérer les ressources
cap.release()
out.release()
cv.destroyAllWindows()
