#include <pipeline/xPipeline.h>
#include <xAssert/xAssert.h>
#include <iostream>
#include <string>


int main()
{
    const std::string tcPipelineStr = "appsrc ! videoconvert ! video/x-raw,format=I420,width=640,height=480,framerate=30/1 ! x264enc tune=zerolatency speed-preset=ultrafast bitrate=500 ! rtph264pay ! udpsink host=127.0.0.1 port=5000";
    int l_iWidth = 640;
    int l_iHeight = 480;
    int l_iFramePerSecond = 30;
    bool l_bColor = true;
    int l_ireturn = 0;

    CxPipeline* l_ptPipeline = nullptr;
    X_ASSERT(CxPipeline::buildInstance(tcPipelineStr, l_iWidth, l_iHeight, l_iFramePerSecond, l_bColor) == 0);
    l_ptPipeline = CxPipeline::getInstance();

    X_ASSERT(l_ptPipeline->initPipeline() == 0);
    l_ptPipeline->runPipeline();

    return 0;
}