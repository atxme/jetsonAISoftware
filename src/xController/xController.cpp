//
// Created by christophe on 8/13/24.
//

#include "xController.h"
#include "xAssert.h"
#include <iostream>
#include <chrono>
#include <thread> // Inclure pour std::this_thread::sleep_for
#include <SDL2/SDL.h> // Assurez-vous d'inclure SDL2

///////////////////////////////////
/// Constructor
///////////////////////////////////
CxController::CxController(CxNetworkServer* p_ptNetworkInstance)
{
    m_pJoystick(nullptr);
    m_fDeadZone(0.1);
    m_pNetworkInstance = p_ptNetworkInstance;
}


///////////////////////////////////
/// Destructor
///////////////////////////////////
CxController::~CxController()
{
    if (m_pJoystick) {
        SDL_JoystickClose(m_pJoystick);
    }
    SDL_Quit();
}

///////////////////////////////////
/// BuildInstance
///////////////////////////////////
int CxController::buildInstance(CxNetworkServer* p_ptNetworkInstance)
{
    if (m_pInstance != nullptr)
    {
        X_ASSERT(false);
    }
    m_pInstance = new CxController(p_ptNetworkInstance);
    return 0;
}

///////////////////////////////////
/// ListControllerInfo
///////////////////////////////////
void CxController::listControllerinfo()
{
    int joystickCount = SDL_NumJoysticks();
    std::cout << "Number of detected controllers: " << joystickCount << std::endl;

    if (joystickCount == 0) {
        std::cerr << "No controllers detected." << std::endl;
        exit(1);
    }

    for (int index = 0; index < joystickCount; ++index) {
        SDL_Joystick* joystick = SDL_JoystickOpen(index);
        if (joystick) {
            std::cout << "Controller " << index << ":" << std::endl;
            std::cout << "  Name: " << SDL_JoystickName(joystick) << std::endl;
            std::cout << "  Number of axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
            std::cout << "  Number of buttons: " << SDL_JoystickNumButtons(joystick) << std::endl;
            std::cout << "  Number of hats: " << SDL_JoystickNumHats(joystick) << std::endl;
            SDL_JoystickClose(joystick);
        }
    }
}

///////////////////////////////////
/// InitController
///////////////////////////////////
int CxController::initController()
{
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    int joystickCount = SDL_NumJoysticks();
    if (joystickCount == 0)
    {
        std::cerr << "No joystick detected." << std::endl;
        return -1;
    }

    m_pJoystick = SDL_JoystickOpen(0);
    if (!m_pJoystick) {
        std::cerr << "Couldn't open joystick: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set default axis values and dead zone
    m_fAxis[0] = m_fAxis[1] = 0;
    return 0;
}

///////////////////////////////////
/// SelfTest
///////////////////////////////////
void CxController::selfTest()
{
    int l_iRun = 0;
    while (l_iRun < 1000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SDL_PumpEvents();

        float vertical_axis_value = SDL_JoystickGetAxis(m_pJoystick, 1) / 32767.0f;
        if (std::abs(vertical_axis_value) < m_fDeadZone) vertical_axis_value = 0;
        m_fAxis[0] = -vertical_axis_value;

        float horizontal_axis_value = SDL_JoystickGetAxis(m_pJoystick, 2) / 32767.0f;
        if (std::abs(horizontal_axis_value) < m_fDeadZone) horizontal_axis_value = 0;
        m_fAxis[1] = horizontal_axis_value;

        std::cout << "Axis: [" << m_fAxis[0] << ", " << m_fAxis[1] << "]" << std::endl;
        l_iRun++;
    }
}

///////////////////////////////////
///runController
///////////////////////////////////
void CxController::runController()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SDL_PumpEvents();

        float vertical_axis_value = SDL_JoystickGetAxis(m_pJoystick, 1) / 32767.0f;
        if (std::abs(vertical_axis_value) < m_fDeadZone) vertical_axis_value = 0;
        m_fAxis[0] = -vertical_axis_value;

        float horizontal_axis_value = SDL_JoystickGetAxis(m_pJoystick, 2) / 32767.0f;
        if (std::abs(horizontal_axis_value) < m_fDeadZone) horizontal_axis_value = 0;
        m_fAxis[1] = horizontal_axis_value;

        //cast the axis float in axis[0] ";" axis[1]
        std::string l_sCommand = std::to_string(m_fAxis[0]) + ";" + std::to_string(m_fAxis[1]);

        m_pNetworkInstance->getInstance().sendUpdate(m_fAxis);
    }
}
