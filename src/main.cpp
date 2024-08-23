#include "xController/xController.h"
#include "xStreamer/xStreaming.h"
#include "xSignature/xSignature.h"
#include "xAssert/xAssert.h"
#include "xNetwork/xNetwork.h"
#include "xNetwork/xSocket.h"
#include <thread>



int checkBinarySignature()
{
    unsigned char l_aucSignature[X_SIGNATURE_SIZE];
    unsigned char* l_aucContent = (unsigned char*)malloc(1024); //this value is arbitrary and should be changed
    long l_lContentSize = 0;
    EVP_PKEY *l_ptucPublicKey = nullptr;

    if (getDataFromFile("server", l_aucSignature, l_aucContent, &l_lContentSize) != 0)
    {
        std::cerr << "Error: unable to get the data from the file" << std::endl;
        return -1;
    }

    if (getPublicKeyFromFile("kPub.pem", &l_ptucPublicKey) != 0)
    {
        std::cerr << "Error: unable to get the public key from the file" << std::endl;
        return -1;
    }

    std::cout << "file signature: " << l_aucSignature << std::endl;
    std::cout << "file content: " << l_aucContent << std::endl;
    std::cout << "file size: " << l_lContentSize << std::endl;

    if (xSignatureCheck(l_aucSignature, l_aucContent, l_lContentSize, l_ptucPublicKey) != 0)
    {
        std::cerr << "Error: signature check failed" << std::endl;
        return -1;
    }

    return 0;
}


int main()
{
    int l_iReturn = 0;
    //socket configuration
    tSocketConfig l_tSocketConfig;
    sockaddr_in l_tSockAddr = {};
    l_tSockAddr.sin_family = AF_INET;
    l_tSockAddr.sin_port = htons(0); // Vous pouvez définir le port par défaut ou le spécifier comme nécessaire
    l_tSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Définit l'adresse IP à accepter toutes les connexions
    memset(l_tSockAddr.sin_zero, 0, sizeof(l_tSockAddr.sin_zero));

    sockaddr_server* l_tSockAddrServer = {0};

    //controller instance
    CxController* l_ptControllerInstance = nullptr;
    CxStreaming* l_ptStreamingInstance = nullptr;

    if (checkBinarySignature() != 0)
    {
        std::cerr << "Error: signature check failed, the binary have been edited or the key is bad" << std::endl;
        std::cerr << "Exiting..." << std::endl;
        return -1;
    }


    l_tSocketConfig.usSocketType = TCP;
    l_tSocketConfig.ulPort = 8080;
    l_tSocketConfig.pcIpAddress =(char*)"127.0.0.1";
    l_tSocketConfig.ptSockAddr = &l_tSockAddr;
    l_tSocketConfig.ptSockAddrServer = l_tSockAddrServer;

    l_iReturn = CxNetwork::buildInstance(&l_tSocketConfig);
    if (l_iReturn != 0)
    {
        std::cerr << "Error: unable to build the network instance" << std::endl;
        std::cerr << "Exiting..." << std::endl;
        return -1;
    }
    
    CxNetwork* l_ptNetworkInstance = CxNetwork::getInstance();
    if (l_ptNetworkInstance == nullptr)
    {
        return -1;
    }

    if (l_ptNetworkInstance->initSocket() != 0)
    {
        std::cerr << "Error: unable to init the network" << std::endl;
        return -1;
    }

    l_iReturn = CxController::buildInstance(l_ptNetworkInstance);

    if (l_iReturn != 0)
    {
        std::cerr << "Error: unable to build the controller instance" << std::endl;
        std::cerr << "Exiting..." << std::endl;
        return -1;
    }

    l_ptControllerInstance = CxController::getInstance();

    if (l_ptControllerInstance->initController() != 0)
    {
        return -1;
    }

    l_ptControllerInstance->listControllerinfo();

    #ifdef _STARTWITHSELFTEST
    l_ptControllerInstance->selfTest();
    #endif


    //gst-launch-1.0 udpsrc port=5000 ! application/x-rtp, jitterbuffer-mode=none ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink
    l_iReturn = CxStreaming::buildInstance("udpsrc port=5000 ! application/x-rtp, jitterbuffer-mode=none ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink");

    if (l_iReturn != 0)
    {
        std::cerr << "Error: unable to build the streaming instance" << std::endl;
        std::cerr << "Exiting..." << std::endl;
        return -1;
    }

    l_ptStreamingInstance = CxStreaming::getInstance();

    if (l_ptStreamingInstance->initStreaming() != 0)
    {
        return -1;
    }

    std::thread l_oControllerThread(&CxController::runController, l_ptControllerInstance);
    std::thread l_oStreamingThread(&CxStreaming::runStreaming, l_ptStreamingInstance);

    l_oControllerThread.join();
    l_oStreamingThread.join();
    
    return 0;
}