//
// Created by christophe on 8/13/24.
//

#ifndef XCONTROLLER_H
#define XCONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <xNetwork.h>

class CxController 
{

public:

    /**
     *@brief build instance of the controller
     *@param p_ptNetworkInstance the network server instance
     *@return 0 if the controller is created successfully, -1 otherwise
     */
     int buildInstance(CxNetwork* p_ptNetworkInstance);


    /**
     * @brief destructor
     */
    ~CxController();

    /**
     * @brief copy constructor forbidden
     */
    CxController(const CxController&) = delete;

    /**
     * @brief copy assignment forbidden
     */
    CxController& operator=(const CxController&) = delete;

    /**
     *@brief list controller available
     */
    void listControllerinfo();

    /*
     *@brief initialize the controller
     *
     *@return 0 if the controller is initialized successfully, -1 otherwise
     */
     int initController();

     /**
      *@brief self test
      *
      */
     void selfTest();

     /**
      *@brief run the controller
      *@details the controller will send the update to the networkServer on a specific thread
      */
     void runController();

private:

    /**
     *@brief singleton
     */
    static CxController* m_pInstance;

    /**
     * @brief constructor
     */
    explicit CxController(CxNetwork* p_ptNetworkInstance);

    /**
     *@brief SDL joystick
     */
    SDL_Joystick* m_pJoystick;

    /**
     *@brief axis float
     */
    float m_fAxis[2];

    /**
     *@brief dead zone
     */
    float m_fDeadZone;

    /**
     *@brief networkServer Instance
     */
    CxNetwork* m_pNetworkInstance;
};

#endif //XCONTROLLER_H
