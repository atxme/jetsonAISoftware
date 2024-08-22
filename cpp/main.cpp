#include <pipeline/xPipeline.h>
#include <xAssert/xAssert.h>
#include <XsSignature/xSignature.h>
#include <iostream>
#include <string>

int checkSignature()
{
    unsigned char l_aucSignature[X_SIGNATURE_SIZE];
    unsigned char* l_pucContent = nullptr;
    long l_lContentSize = 0;
    EVP_PKEY* l_ptucPublicKey = nullptr;
    int l_iReturn = 0;

    // Lire les données et la signature depuis le fichier
    l_iReturn = getDataFromFile("libstreamer.so", l_aucSignature, l_pucContent, &l_lContentSize);
    if (l_iReturn != 0) {
        fprintf(stderr, "Erreur lors de la lecture du fichier\n");
        return -1;
    }

    // Lire la clé publique depuis le fichier PEM
    l_iReturn = getPublicKeyFromFile("kPub.pem", &l_ptucPublicKey);
    if (l_iReturn != 0) {
        fprintf(stderr, "Erreur lors de la lecture de la clé publique\n");
        free(l_pucContent);
        return -1;
    }

    // Vérifier la signature
    l_iReturn = xSignatureCheck(l_aucSignature, l_pucContent, l_lContentSize, l_ptucPublicKey);
    if (l_iReturn != 0) {
        fprintf(stderr, "Erreur lors de la vérification de la signature\n");
    }

    // Libérer la mémoire allouée pour la clé publique et le contenu
    EVP_PKEY_free(l_ptucPublicKey);
    free(l_pucContent);

    return l_iReturn;
}

int main()
{

    const std::string tcPipelineStr = "appsrc ! videoconvert ! video/x-raw,format=I420,width=640,height=480,framerate=30/1 ! x264enc tune=zerolatency speed-preset=ultrafast bitrate=500 ! rtph264pay ! udpsink host=127.0.0.1 port=5000";
    int l_iWidth = 640;
    int l_iHeight = 480;
    int l_iFramePerSecond = 30;
    bool l_bColor = true;
    int l_ireturn = 0;

    CxPipeline* l_ptPipeline = nullptr;
    X_ASSERT(CxPipeline::buildInstance(tcPipelineStr, l_iWidth, l_iHeight, l_iFramePerSecond, l_bColor) == 0);
    l_ptPipeline = CxPipeline::getInstance();

    X_ASSERT(l_ptPipeline->initPipeline() == 0);
    l_ptPipeline->runPipeline();

    return 0;
}