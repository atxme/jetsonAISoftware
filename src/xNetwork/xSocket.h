//
// Created by christophe on 6/30/24.
//

#ifndef XSOCKET_H
#define XSOCKET_H

#ifdef cplusplus
extern "C" {
#endif //cplusplus

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#else

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#endif // _WIN32

// Socket types
#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM

#define MAX_BUFFER_SIZE 1024
#define MAX_SOCKET_AVAILABLE 10

/**
 * @brief Socket configuration structure
 * @details This structure is used to configure a socket
 * 
 * @param usSocketType : Socket type (e.g., SOCK_STREAM for TCP)
 * @param ulPort : Port number (better to use in_port_t for port)
 * @param pcIpAddress : IP address as a string
 * @param ptSockAddr : Pointer to a sockaddr_in structure
 * @param ptSockAddrServer : Pointer to a sockaddr_server structure only define for connection
 */
typedef struct tSocketConfig {
    unsigned short usSocketType;    // Socket type (e.g., SOCK_STREAM for TCP)
    in_port_t ulPort;               // Port number (better to use in_port_t for port)
    char* pcIpAddress;              // IP address as a string
    struct sockaddr_in* ptSockAddr; // Pointer to a sockaddr_in structure
    struct sockaddr_server* ptSockAddrServer; // Pointer to a sockaddr_server structure only define for connection
} tSocketConfig;

struct tData
{
    char* pcData; // Data to send or receive (use stack memory instead of heap memory)
    unsigned long ulDataSize; // Data size to wait or send (ulDataSize > 0 && ulDataSize < MAX_BUFFER_SIZE)
};

#ifdef CRC_ON_SOCKET

#define CRC16_POLYNOMIAL 0x8005

/*
 * @brief CRC 16 calculation function
 *
 * @param p_pucData : pointer to the data
 * @param p_ulDataSize : size of the data
 * @param p_usCrc : pointer to the CRC value
 *
 * @note This function is used to calculate the CRC 16 of a data
 *
 * @return 0 if the CRC is calculated successfully, -1 otherwise
 */
int Crc16Processing(unsigned char* p_pucData, unsigned long p_ulDataSize, unsigned short* p_usCrc);
#endif //CRC_ON_SOCKET

/*
 * @brief Socket creation function
 * @note This function is used to create a socket
 *
 * @param p_ptSocketConfig : pointer to the socket configuration structure
 *
 * @return socket if the socket is created successfully, -1 otherwise
 * @note The function returns the socket file descriptor
* @note if def ASSERT_SOCKET and an error accurs, the function will assert and stop the software execution
 */
int createSocket(struct tSocketConfig* p_ptSocketConfig);


/*
 * @brief Socket deletion function
 * @note This function is used to delete a socket
 *
 * @param p_iSocket : socket file descriptor
 *
 * @return 0 if the socket is deleted successfully, -1 otherwise
 */
int deleteSocket(int p_iSocket);

/*
 * @brief Socket connection function
 * @note This function is used to connect a socket
 *
 * @param p_iSocket : reference to the socket file descriptor
 * @param p_ptSocketConfig : pointer to the socket configuration structure which includes the server IP address and port
 *
 * @return 0 if the socket is connected successfully, -1 otherwise
 */
int connectSocket(int p_iSocket, struct tSocketConfig* p_ptSocketConfig);

/*
 * @brief Socket send function
 * @note This function is used to send data through a socket
 *
 * @param p_ptData : pointer to the data structure
 * @param p_iSocket : socket file descriptor
 *
 * @return 0 if the data is sent successfully, -1 otherwise
 */
int sendData(struct tData* p_ptData, int p_iSocket);

/*
 * @brief Socket receive function
 * @note this function is used to receive data from a socket
 *
 * @details We recommand to use a thread to receive data from a socket
 *
 * @param p_ptData : pointer to the data structure
 * @param p_iSocket : socket file descriptor
 *
 * @return 0 if the data is received successfully, -1 otherwise
 */
int receiveData(struct tData* p_ptData, int p_iSocket);



#ifdef cplusplus
}
#endif //cplusplus

#endif //XSOCKET_H
