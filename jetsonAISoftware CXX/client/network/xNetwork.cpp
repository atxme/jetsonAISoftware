//
// Created by christophe on 8/13/24.
//

#include "xNetwork.h"
#include "xAssert.h"
#include <thread>
#include <iostream>

CxNetwork* CxNetwork::m_pInstance = nullptr;

///////////////////////////////////
/// CxNetwork
///////////////////////////////////
CxNetwork::CxNetwork(tSocketConfig *p_ptSocketConfig)
{
    int l_iSocket = createSocket(p_ptSocketConfig);
    m_vSocket.push_back(l_iSocket);
    tSocketConfig l_tSocketConfig = *p_ptSocketConfig;
    m_vSocketConfig.push_back(l_tSocketConfig);
}

///////////////////////////////////
/// ~CxNetwork
///////////////////////////////////
CxNetwork::~CxNetwork()
{
    for (int l_iSocket : m_vSocket)
    {
        deleteNetworkSocket(l_iSocket);
    }
}

///////////////////////////////////
/// buildInstance
///////////////////////////////////
int CxNetwork::buildInstance(struct tSocketConfig* p_ptSocketConfig)
{
    X_ASSERT(p_ptSocketConfig != nullptr)
    if (m_pInstance == nullptr)
    {
        m_pInstance = new CxNetwork(p_ptSocketConfig);
        return 0;
    }
    return -1;
}


///////////////////////////////////
/// getInstance
///////////////////////////////////
CxNetwork* CxNetwork::getInstance()
{
    if (m_pInstance == nullptr)
    {
        X_ASSERT(false);
    }
    return m_pInstance;
}

///////////////////////////////////
/// deleteNetworkSocket
///////////////////////////////////
int CxNetwork::deleteNetworkSocket(int p_iSocket)
{
    for (int l_iIndex = 0; l_iIndex < m_iSocketCount; l_iIndex++)
    {
        if (m_vSocket[l_iIndex] == p_iSocket)
        {
            m_vSocket.erase(m_vSocket.begin() + l_iIndex);
            m_iSocketCount--;
        }
    }
    return deleteSocket(p_iSocket);
}


///////////////////////////////////
/// getSocketCount
///////////////////////////////////
int CxNetwork::getSocketCount() const
{
    return m_iSocketCount;
}

///////////////////////////////////
/// getSocketTable
////////////////////////////////////
void CxNetwork::getSocketTable(std::vector<int>& p_pvSocket)
{
    X_ASSERT(p_pvSocket.empty());
    for (int l_iSocket : m_vSocket)
    {
        p_pvSocket.push_back(l_iSocket);
    }
}

///////////////////////////////////
/// run
///////////////////////////////////
void CxNetwork::run()
{
    int l_iReturn = 0;
    for (int l_iCounter = 0; l_iCounter < m_iSocketCount; l_iCounter++)
    {
        l_iReturn = connectSocket(m_vSocket[l_iCounter], &m_vSocketConfig[l_iCounter]);

        if (l_iReturn < 0)
        {
            std::cout << "Error connecting socket" << std::endl;
            std::cout << "Socket configuration : " << m_vSocketConfig[l_iCounter].usSocketType << std::endl;
        }
    }
}



