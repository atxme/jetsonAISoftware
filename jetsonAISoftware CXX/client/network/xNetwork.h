//
// Created by christophe on 8/13/24.
//

#ifndef XNETWORK_H
#define XNETWORK_H

#ifndef ASSERT_SOCKET
#define ASSERT_SOCKET
#endif //ASSERT_SOCKET

#include "xSocket.h"
#include <vector>


class CxNetwork
{

public :

    /**
     * @brief build instance of the network
     * @param struct tSocketConfig : socket configuration
     *
     * @return 0 if the network is created successfully, -1 otherwise
     */
    static int buildInstance(struct tSocketConfig* p_ptSocketConfig);

    /**
     * @brief destructor
     */
    ~CxNetwork();

    /**
     * @brief copy constructor forbidden
     */
    CxNetwork(const CxNetwork&) = delete;

    /**
     * @brief copy assignment forbidden
     */
    CxNetwork& operator=(const CxNetwork&) = delete;

    /**
     * @brief get the instance of the network
     *
     * @return the instance of the network
     */
    static CxNetwork* getInstance();

    /**
     * @brief delete socket
     * @param p_iSocket : socket to delete
     * @return 0 if the socket is deleted successfully, -1 otherwise
     */
    int deleteNetworkSocket(int p_iSocket);

    /**
     * @brief get the number of available socket
     * @return the number of available socket
     */
    int getSocketCount() const;

    /**
     * @brief get the socket table
     * @param p_pvSocket : vector of socket
     */
    void getSocketTable(std::vector<int>& p_pvSocket);


    /**
     * @brief run network
     * @details this function is started in a thread
     */
    void run();

private :

    /**
     * @brief constructor
     * @param tSocketConfig : socket configuration
     */
    explicit CxNetwork(struct tSocketConfig* p_ptSocketConfig);


    /**
     * singleton instance
     */
    static CxNetwork* m_pInstance;

    /**
     * counter for available socket
     */
    int m_iSocketCount = 0;

    /**
     * vector of socket
     */
    std::vector<int> m_vSocket;

   /**
    * vector of socket configuration
    */
    std::vector<struct tSocketConfig> m_vSocketConfig;

};

#endif //XNETWORK_H
