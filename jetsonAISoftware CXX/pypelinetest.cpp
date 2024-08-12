#include <opencv4/opencv2/opencv.hpp>
#include <gst/gst.h>
#include <iostream>

void handleGstMessage(GstBus *bus, GstMessage *msg) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;
            gst_message_parse_error(msg, &err, &debug_info);
            std::cerr << "GStreamer error: " << err->message << std::endl;
            std::cerr << "Debug info: " << (debug_info ? debug_info : "none") << std::endl;
            g_error_free(err);
            g_free(debug_info);
            break;
        }
        case GST_MESSAGE_EOS:
            std::cout << "End of stream" << std::endl;
            break;
        default:
            break;
    }
}

int main(int argc, char* argv[]) {
    gst_init(&argc, &argv);

    // Initialiser la capture vidéo (0 pour la première caméra)
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Erreur: Impossible d'ouvrir la caméra" << std::endl;
        return -1;
    }

    // Définir les paramètres du pipeline GStreamer
    const std::string pipeline_str = 
    "appsrc ! videoconvert ! video/x-raw,format=I420,width=640,height=480,framerate=30/1 ! x264enc tune=zerolatency speed-preset=ultrafast bitrate=500 ! rtph264pay ! udpsink host=127.0.0.1 port=5000";


    // Ouvrir le pipeline GStreamer
    cv::VideoWriter writer(pipeline_str, cv::CAP_GSTREAMER, 0, 30, cv::Size(640, 480), true);
    if (!writer.isOpened()) {
        std::cerr << "Erreur: Impossible d'ouvrir le pipeline GStreamer" << std::endl;
        return -1;
    }

    std::cout << "Pipeline GStreamer ouvert avec succès." << std::endl;

    // Boucle principale
    while (true) {
        cv::Mat frame;
        cap >> frame;  // Capturer une image

        if (frame.empty()) {
            std::cerr << "Aucune image capturée, arrêt." << std::endl;
            break;  // Arrêter si aucune image n'est capturée
        }

        writer.write(frame);  // Envoyer l'image au pipeline GStreamer
        cv::imshow("Frame", frame);  // Afficher l'image capturée

        if (cv::waitKey(1) == 'q') {
            std::cout << "Sortie de la boucle, 'q' pressé." << std::endl;
            break;  // Sortir de la boucle si 'q' est pressé
        }
    }

    // Libérer les ressources
    cap.release();
    writer.release();
    cv::destroyAllWindows();

    return 0;
}
