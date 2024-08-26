#include "xController/xController.h"
#include "xStreamer/xStreaming.h"
#include "xSignature/xSignature.h"
#include "xAssert/xAssert.h"
#include "xNetwork/xNetwork.h"
#include "xNetwork/xSocket.h"
#include <thread>




int checkBinarySignature()
{
    unsigned char* l_aucSignature = (unsigned char*)malloc(132); //this value is arbitrary and is edited by the code
    unsigned char* l_aucContent = (unsigned char*)malloc(52400); //this value is arbitrary and is edited by the code
    long l_lContentSize = 0;
    long l_lSignatureSize = 0;
    EVP_PKEY *l_ptucPublicKey = nullptr;

    if (getDataFromFile("server", l_aucSignature, l_aucContent, &l_lContentSize, &l_lSignatureSize) != 0)
    {
        std::cerr << "Error: unable to get the data from the file" << std::endl;
        return -1;
    }

    if (getPublicKeyFromFile("kPub.pem", &l_ptucPublicKey) != 0)
    {
        std::cerr << "Error: unable to get the public key from the file" << std::endl;
        return -1;
    }

    if (xSignatureCheck(l_aucSignature, l_aucContent, l_lContentSize, l_lSignatureSize, l_ptucPublicKey) != 0)
    {
        std::cerr << "Error: signature check failed" << std::endl;
        return -1;
    }

    //free the memory
    free(l_aucSignature);
    free(l_aucContent);

    return 0;
}


int main()
{
    int l_iReturn = 0;
    //socket configuration
    tSocketConfig l_tSocketConfig = {};
    sockaddr_in l_tSockAddr = {};
    l_tSockAddr.sin_family = AF_INET;
    l_tSockAddr.sin_port = htons(0); // Vous pouvez définir le port par défaut ou le spécifier comme nécessaire
    l_tSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Définit l'adresse IP à accepter toutes les connexions
    memset(l_tSockAddr.sin_zero, 0, sizeof(l_tSockAddr.sin_zero));

    sockaddr_server* l_tSockAddrServer = {0};

    //controller instance
    CxController* l_ptControllerInstance = nullptr;
    CxStreaming* l_ptStreamingInstance = nullptr;

/*
    if (checkBinarySignature() != 0)
    {
        std::cerr << "Error: signature check failed, the binary have been edited or the key is bad" << std::endl;
        std::cerr << "Exiting..." << std::endl;
        return -1;
    }
*/

    l_tSocketConfig.usSocketType = TCP;
    l_tSocketConfig.ulPort = 8080;
    l_tSocketConfig.pcIpAddress =(char*)"10.82.251.197";
    l_tSocketConfig.ptSockAddr = &l_tSockAddr;
    l_tSocketConfig.ptSockAddrServer = l_tSockAddrServer;
    l_tSockAddr.sin_port = htons(l_tSocketConfig.ulPort);
    inet_pton(AF_INET, l_tSocketConfig.pcIpAddress, &l_tSockAddr.sin_addr);


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


    #ifdef TEST_MODE
    l_iReturn = CxStreaming::buildInstance("videotestsrc ! videoconvert ! autovideosink");
    #else 
    l_iReturn = CxStreaming::buildInstance("udpsrc port=5000 ! application/x-rtp, jitterbuffer-mode=none ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink ! appsink");
    #endif
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