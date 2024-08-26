//
// Created by christophe on 6/30/24.
// project : jetson software server
//

#include <xStreaming.h>
#include <iostream>
#include <xAssert.h>
#include <gst/gst.h>
#include <opencv4/opencv2/opencv.hpp>

CxStreaming* CxStreaming::m_ptInstance = nullptr;

///////////////////////////////////
/// constructor
///////////////////////////////////
CxStreaming::CxStreaming(std::string p_tcPipelineStr)
{
    X_ASSERT(!p_tcPipelineStr.empty());
    m_tcPipelineStr = p_tcPipelineStr;
}

///////////////////////////////////
/// destructor
///////////////////////////////////
CxStreaming::~CxStreaming()
{
    if (m_oVideoCapture.isOpened() == true)
    {
        m_oVideoCapture.release();
    }

    cv::destroyAllWindows();
}

///////////////////////////////////
/// buildInstance
///////////////////////////////////
int CxStreaming::buildInstance(std::string p_tcPipelineStr)
{
    if (m_ptInstance != nullptr)
    {
        X_ASSERT(false);
    }
    m_ptInstance = new CxStreaming(p_tcPipelineStr);

    return 0;
}

///////////////////////////////////
/// getInstance
///////////////////////////////////
CxStreaming* CxStreaming::getInstance()
{
    if (m_ptInstance == nullptr)
    {
        X_ASSERT(false);
    }
    return m_ptInstance;
}

///////////////////////////////////
/// initStreaming
///////////////////////////////////
int CxStreaming::initStreaming()
{
    gst_init(nullptr, nullptr);
    //init capture video
    std::cout << "Pipeline string : " << m_tcPipelineStr << std::endl;
    m_oVideoCapture = cv::VideoCapture(m_tcPipelineStr, cv::CAP_GSTREAMER);
    if (m_oVideoCapture.isOpened() == false)
    {
        std::cerr << "Error: Unable to open the camera" << std::endl;
        return -1;
    }

    return 0;
}


///////////////////////////////////
/// runStreaming
///////////////////////////////////
void CxStreaming::runStreaming()
{
    cv::Mat l_oFrame;
    while (true)
    {
        m_oVideoCapture >> l_oFrame;  // Capture an image
        m_oFrame = l_oFrame;

        if(l_oFrame.empty() == true)
        {
            std::cerr << "No image captured, stopping." << std::endl;
            break;
        }

        cv::imshow("Streaming capture", l_oFrame);
    }
}





