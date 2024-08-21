//
// Created by christophe on 6/30/24.
// project : jetson software client
//
#ifndef xPipeline
#define xPipeline


#include <opencv4/opencv2/opencv.hpp>
#include <string>

class CxPipeline
{
public :

    /**
     * @brief build Instance
     * @param p_tcPipelineStr : gstreamer command
     * @param p_iWidth : width of the frame
     * @param p_iHeight : height of the frame
     * @param p_iFramePerSecond : frame per second to capture
     * @param p_bColor : true for BGR false FOR WB
     *
     * @return 0 if valid -1 otherwise
     */
    static int buildInstance(std::string p_tcPipelineStr,
                       int p_iWidth,
                       int p_iHeight,
                       int p_iFramePerSecond,
                       bool p_bColor);

     /**
      * @brief destructor
      * @detail release all the parameter used for the pipeline
      */
     ~CxPipeline();


     /**
      * @brief copy constructor forbidden
      */
     CxPipeline(const CxPipeline&) = delete;

     /**
      *@brief copy issignement forbidden
      */
     CxPipeline& operator = (const CxPipeline&) = delete;

     /**
      *@brief get instance of the singleton
      */
      static CxPipeline* getInstance();

     /**
      *@brief init all parammeter for the pipeline
      */
     int initPipeline();

     /**
      *@brief run pipeline
      */
     void runPipeline();

private :

    /**
     * constructor
     *
     * @brief build Instance
     * @param p_tcPipelineStr : gstreamer command
     * @param p_iWidth : width of the frame
     * @param p_iHeight : height of the frame
     * @param p_iFramePerSecond : frame per second to capture
     * @param p_bColor : true for BGR false FOR WB
     */
     CxPipeline(std::string p_tcPipelineStr,
                            int p_iWidth,
                            int p_iHeight,
                            int p_iFramePerSecond,
                            bool p_bColor);


    /**
     * singleton
     */
    static CxPipeline* m_ptInstance;

    /**
     * @brief opencv cap
     * @detail used to capture image from a camera
     */
    cv::VideoCapture m_oVideoCapture;

    /**
     * @brief opencv video writer
     * @detail used to encode and send the image capture with the cap throught the pipeline
     */
    cv::VideoWriter m_oVideoWriter;

    /**
     *@brief gstreamer command
     */
     std::string m_tcPipelineStr;

    /**
     *@brief width of the image to be capture
     */
    int m_iWidth;

    /**
     *@brief height of the image to be capture
     */
    int m_iHeight;

    /**
     *@brief frame per second for the streaming
     */
    int m_iFramePerSecond;

    /**
     *@brief bool color
     */
    bool m_bColor;
};




#endif