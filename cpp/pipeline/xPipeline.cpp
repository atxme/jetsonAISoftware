//
// Created by christophe on 6/30/24.
// project : jetson software client
//

#include "xPipeline.h"
#include <iostream>
#include "xAssert.h"
#include <gst/gst.h>
#include <string>

CxPipeline* CxPipeline::m_ptInstance = nullptr;

///////////////////////////////////
/// constructor
///////////////////////////////////
CxPipeline::CxPipeline(std::string p_tcPipelineStr,
                       int p_iWidth,
                       int p_iHeight,
                       int p_iFramePerSecond,
                       bool p_bColor)
{
    X_ASSERT(!p_tcPipelineStr.empty());
    X_ASSERT(p_iWidth != 0);
    X_ASSERT(p_iHeight != 0);
    X_ASSERT(p_iFramePerSecond != 0);
    X_ASSERT(p_bColor == true || p_bColor == false);

    m_tcPipelineStr = p_tcPipelineStr;
    m_iWidth = p_iWidth;
    m_iHeight = p_iHeight;
    m_iFramePerSecond = p_iFramePerSecond;
    m_bColor = p_bColor;
}

///////////////////////////////////
/// destructor
///////////////////////////////////
CxPipeline::~CxPipeline()
{
    if (m_oVideoCapture.isOpened() == true)
    {
        m_oVideoCapture.release();
    }

    if (m_oVideoWriter.isOpened() == true)
    {
        m_oVideoWriter.release();
    }

    cv::destroyAllWindows();
}

///////////////////////////////////
/// buildInstance
///////////////////////////////////
int CxPipeline::buildInstance(std::string p_tcPipelineStr,
                              int p_iWidth,
                              int p_iHeight,
                              int p_iFramePerSecond,
                              bool p_bColor)
{
    if (m_ptInstance != nullptr)
    {
        X_ASSERT(false);
    }
    m_ptInstance = new CxPipeline(p_tcPipelineStr, p_iWidth, p_iHeight, p_iFramePerSecond, p_bColor);

    return 0;
}

///////////////////////////////////
/// getInstance
///////////////////////////////////
CxPipeline* CxPipeline::getInstance()
{
    if (m_ptInstance == nullptr)
    {
        X_ASSERT(false);
    }
    return m_ptInstance;
}

///////////////////////////////////
/// initPipeline
///////////////////////////////////
int CxPipeline::initPipeline()
{
    gst_init(nullptr, nullptr);
    //init capture video
    m_oVideoCapture = cv::VideoCapture(0);
    if (m_oVideoCapture.isOpened() == false)
    {
        std::cerr << "Error: Unable to open the camera" << std::endl;
        return -1;
    }

    //init video writer
    m_oVideoWriter = cv::VideoWriter(m_tcPipelineStr,
                                     cv::CAP_GSTREAMER,
                                     0, m_iFramePerSecond,
                                     cv::Size(m_iWidth, m_iHeight),
                                     m_bColor);

    if (m_oVideoWriter.isOpened() == false)
    {
        std::cerr << "Error: Unable to open the GStreamer pipeline" << std::endl;
        return -1;
    }

    std::cout << "GStreamer pipeline opened successfully." << std::endl;

    return 0;
}

///////////////////////////////////
/// runPipeline
///////////////////////////////////
void CxPipeline::runPipeline()
{
    cv::Mat l_oFrame;
    while (true)
    {
        m_oVideoCapture >> l_oFrame;  // Capture an image
        if(l_oFrame.empty() == true)
        {
            std::cerr << "No image captured, stopping." << std::endl;
            break;
        }

        m_oVideoWriter.write(l_oFrame);  // Send the image to the GStreamer pipeline
    }
}

