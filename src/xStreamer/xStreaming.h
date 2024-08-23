//
// Created by christophe on 6/30/24.
// project : jetson software server
//


#ifndef xStreaming 
#define xStreaming


#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <gst/gst.h>

/**
 * @brief class to get stream video send by the client 
 */
class CxStreaming
{


    public :

        /**
         * @brief build Instance
         * @param p_tcPipelineStr : gstreamer command   
         *
         * @return 0 if valid -1 otherwise
         */
        static int buildInstance(std::string p_tcPipelineStr);

        /**
         * @brief destructor
         * @detail release all the parameter used for the pipeline
         */
        ~CxStreaming();


        /**
         * @brief copy constructor forbidden
         */
        CxStreaming(const CxStreaming&) = delete;

        /**
         *@brief copy issignement forbidden
         */
        CxStreaming& operator = (const CxStreaming&) = delete;

        /**
         *@brief get instance of the singleton
         */
        static CxStreaming* getInstance();

        /**
         *@brief init all parammeter for the pipeline
         */
        int initStreaming();

        /**
         *@brief run pipeline
         */
        void runStreaming();

    private :

        /**
         * @brief constructor
         * @param p_tcPipelineStr : gstreamer command
         */
        CxStreaming(std::string p_tcPipelineStr); 
        

        /**
         * singleton
         */
        static CxStreaming* m_ptInstance;

        /**
         * @brief opencv cap
         * @detail used to capture image from a camera
         */
        cv::VideoCapture m_oVideoCapture;

        /**
         * @brief pippeline string 
         */
        std::string m_tcPipelineStr;

        /**
         * @brief Matrix for neural network inference 
         */
        cv::Mat m_oFrame;


};




#endif