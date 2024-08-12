//
// Created by christophe on 6/30/24.
//

#include "xSocket.h"
#include <stdio.h>
#include "../xAssert/xAssert.h"
#include <string.h>




//////////////////////////
/// createSocket
//////////////////////////
int createSocket(struct tSocketConfig* p_ptSocketConfig)
{
    //counter to check the number of socket created
    static unsigned short l_usSocketCount = 0;
    X_ASSERT(l_usSocketCount < MAX_SOCKET_AVAILABLE);
    //check parameters
    X_ASSERT(p_ptSocketConfig != NULL);

    tSocketConfig l_tSocketConfig = *p_ptSocketConfig;

    //check if socket type is valid for UDP or TCP
    X_ASSERT(l_tSocketConfig.usSocketType == SOCK_STREAM || l_tSocketConfig.usSocketType == SOCK_DGRAM);
    X_ASSERT(l_tSocketConfig.ulPort > 0);
    X_ASSERT(l_tSocketConfig.pcIpAddress != NULL);
    X_ASSERT(l_tSocketConfig.ptSockAddr != NULL);

    //check sockaddr struct parameters
    X_ASSERT(l_tSocketConfig.ptSockAddr->sin_family == AF_INET);

    //check sockaddr conf
    X_ASSERT(l_tSocketConfig.ptSockAddr->sin_port == htons(l_tSocketConfig.ulPort));
    X_ASSERT(inet_pton(AF_INET, l_tSocketConfig.pcIpAddress, &l_tSocketConfig.ptSockAddr->sin_addr) == 1);

    int l_iSocket = socket(AF_INET, l_tSocketConfig.usSocketType, 0);

    if (l_iSocket < 0)
    {
#ifdef ASSERT_SOCKET
        X_ASSERT(false); // assert the socket creation and en d the software execution check log if necessary
#else
        return -1;
#endif //ASSERT_SOCKET
    }

    int l_ireturn = bind(l_iSocket, (struct sockaddr*)l_tSocketConfig.ptSockAddr, sizeof(struct sockaddr_in));

    if (l_ireturn < 0) {
#ifdef ASSERT_SOCKET
        X_ASSERT(false); // assert the socket creation and en d the software execution check log if necessary
#else
        return -1;
#endif //ASSERT_SOCKET
    }
    l_usSocketCount++;
    return l_iSocket;
}


//////////////////////////
/// connectSocket
//////////////////////////
int connectSocket(int p_iSocket, struct tSocketConfig* p_ptSocketConfig) {
    // Check parameters
    X_ASSERT(p_iSocket >= 0);  // Socket must be valid
    X_ASSERT(p_ptSocketConfig != NULL);

    // Retrieve server socket configuration
    tSocketConfig l_tSocketConfig = *p_ptSocketConfig;

    // Ensure server configuration is provided
    X_ASSERT(l_tSocketConfig.ptSockAddrServer != NULL);

    // Connect to the server using the provided socket descriptor and server address
    int l_iResult = connect(p_iSocket,
                            (struct sockaddr*)l_tSocketConfig.ptSockAddrServer,
                            sizeof(struct sockaddr_in));
    if (l_iResult < 0) {
#ifdef ASSERT_SOCKET
        X_ASSERT(false); // Assert the connection failure and end the software execution, check log if necessary
#else
        return -1; // Return error code if not using assertions
#endif //ASSERT_SOCKET
    }
    return 0;  // Return 0 on successful connection
}


//////////////////////////
///sendData
//////////////////////////
int sendData(struct tData* p_ptData, int p_iSocket)
{
    //check input parameters
    X_ASSERT(p_ptData != NULL);
    X_ASSERT(p_ptData->pcData != NULL);
    X_ASSERT(p_ptData->ulDataSize > 0);
    X_ASSERT(p_ptData->ulDataSize < MAX_BUFFER_SIZE);
    X_ASSERT(p_iSocket >= 0);
#ifdef CRC_ON_SOCKET
    //create local variable to store the data with the CRC
    char l_acData[MAX_BUFFER_SIZE + 2];
    unsigned short l_usCrc = 0;

    //calculate CRC
    Crc16Processing((unsigned char*)p_ptData->pcData, p_ptData->ulDataSize, &l_usCrc);

    //copy CRC at the end of the frame
    memcpy(l_acData, p_ptData->pcData, p_ptData->ulDataSize);
    memcpy(l_acData + p_ptData->ulDataSize, &l_usCrc, sizeof(unsigned short));

#else
    //copy data
    char l_acData[MAX_BUFFER_SIZE];
    memcpy(l_acData, p_ptData->pcData, p_ptData->ulDataSize);

#endif

    //send data
    int l_iResult = send(p_iSocket, l_acData, p_ptData->ulDataSize, 0);

    if (l_iResult < 0)
    {
#ifdef ASSERT_SOCKET
        X_ASSERT(false); // assert the data sending and end the software execution check log if necessary
#else
        printf("Error sending data on sock %d\n", p_iSocket);
        return -1;
#endif //ASSERT_SOCKET
    }
    return 0;
}

//////////////////////////
///receiveData
//////////////////////////
int receiveData(struct tData* p_ptData, int p_iSocket)
{
    //check input parameters
    X_ASSERT(p_ptData != NULL);
    X_ASSERT(p_ptData->pcData != NULL);
    X_ASSERT(p_ptData->ulDataSize > 0);
    X_ASSERT(p_ptData->ulDataSize < MAX_BUFFER_SIZE);
    X_ASSERT(p_iSocket >= 0);

    //receive data
    int l_iResult = recv(p_iSocket, p_ptData->pcData, p_ptData->ulDataSize, 0);

    if (l_iResult < 0)
    {
#ifdef ASSERT_SOCKET
        X_ASSERT(false); // assert the data receiving and end the software execution check log if necessary
#else
        printf("Error receiving data on sock %d\n", p_iSocket);
        return -1;
    }

    return 0;
}
#endif //ASSERT_SOCKET


#ifdef CRC_ON_SOCKET

//////////////////////////
/// Crc16Processing
//////////////////////////
int Crc16Processing(unsigned char* p_pucData, unsigned long p_ulDataSize, unsigned short* p_usCrc)
{
    //check parameters
    X_ASSERT(p_pucData != NULL);
    X_ASSERT(p_ulDataSize != 0);
    X_ASSERT(p_usCrc != NULL);

    unsigned short l_usCrc = 0;
    unsigned long l_ulDataSize = p_ulDataSize;

    //initialize CRC
    l_usCrc = 0xFFFF;


    //calculate CRC
    while (l_ulDataSize--)
    {
        l_usCrc ^= *p_pucData++ << 8;

        for (unsigned char l_ucBit = 0; l_ucBit < 8; l_ucBit++)
        {
            if (l_usCrc & 0x8000)
            {
                l_usCrc = (l_usCrc << 1) ^ CRC16_POLYNOMIAL;
            }
            else
            {
                l_usCrc <<= 1;
            }
        }
    }

    //store CRC
    memcpy(p_usCrc, &l_usCrc, sizeof(unsigned short));

    //clear memory
    memset(&l_usCrc, 0, sizeof(unsigned short));

    return 0;
}

#endif //CRC_ON_SOCKET