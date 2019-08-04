// This recording test is basically equivalent to the test code creted by
// Jetson Hacks.  The only modifications were to set a recording path
// for an .mp4 file
// simple_camera.cpp
// MIT License
// Copyright (c) 2019 JetsonHacks
// See LICENSE for OpenCV license and additional information
// Using a CSI camera (such as the Raspberry Pi Version 2) connected to a
// NVIDIA Jetson Nano Developer Kit using OpenCV
// Drivers for the camera and OpenCV are included in the base image

// #include <iostream>
#include <opencv2/opencv.hpp>
// #include <opencv2/videoio.hpp>
// #include <opencv2/highgui.hpp>

#define CAM_NANO false

std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
            std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
            "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
            std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main(int argc, char* argv[])
{
    //Parse Command Line Arguments
    std::cout << argv[0] << std::endl;
    std::string vidFileOut = "";
    if (argc < 2){
        std::cout << "Please Provide An Output File Path" << std::endl;
        return -1;
    } else {
        vidFileOut = std::string(argv[1]);
        std::cout << "Saving Recording to: " << argv[1] << std::endl;
    }

    //Gstreamer Pipeline Parameters
    std::cout << "Preparing to Open Camera..." << std::endl;
    int capture_width = 1280 ;
    int capture_height = 720 ;
    int display_width = 1280 ;
    int display_height = 720 ;
    int framerate = 60 ;
    int flip_method = 0 ;

    std::string pipeline = gstreamer_pipeline(capture_width,
                                              capture_height,
                                              display_width,
                                              display_height,
                                              framerate,
                                              flip_method);
    std::cout << "Using pipeline: \n\t" << pipeline << "\n";

    //Image Capture Device
    cv::VideoCapture* cap;
    if(CAM_NANO){
        cap = new cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
    } else {
        cap = new cv::VideoCapture(0);
    }
    if(!cap->isOpened()) {
        std::cout<<"Failed to open camera."<<std::endl;
        return (-1);
    }

    //Image Display Window
    cv::namedWindow("CSI Camera", cv::WINDOW_AUTOSIZE);
    cv::Mat img;

    //Image to Video
    int frame_width = cap->get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = cap->get(CV_CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter vidOut(vidFileOut,CV_FOURCC('M','J','P','G'),10, cv::Size(frame_width,frame_height));

    std::cout << "Hit ESC to exit" << "\n" ;
    while(true)
    {
        if (!cap->read(img)) {
            std::cout<<"Capture read error"<<std::endl;
            break;
        }

        vidOut.write(img);

        cv::imshow("CSI Camera",img);
        int keycode = cv::waitKey(30) & 0xff ;
        if (keycode == 27) break ;
    }

    cap->release();
    delete cap;
    vidOut.release();
    cv::destroyAllWindows() ;
    return 0;
}


