/**
 * @file bg_sub.cpp
 * @brief Background subtraction tutorial sample code
 * @author Domenico D. Bloisi
 */

#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <functional>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

auto timeStart = std::chrono::system_clock::now();
cv::RNG rng(12345);


const char* params
    = "{ help h         |                   | Print usage }"
      "{ input          | ../data/test0.mp4 | Path to a video or a sequence of image }"
      "{ algo           | MOG2              | Background subtraction method (KNN, MOG2) }";

int main(int argc, char* argv[])
{
    CommandLineParser parser(argc, argv, params);
    parser.about( "This program shows how to use background subtraction methods provided by "
                  " OpenCV. You can process both videos and images.\n" );
    if (parser.has("help"))
    {
        //print help information
        parser.printMessage();
    }

    //! [create]
    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub;
    if (parser.get<String>("algo") == "MOG2")
        pBackSub = createBackgroundSubtractorMOG2();
    else
        pBackSub = createBackgroundSubtractorKNN();
    //! [create]

    //! [capture]
    VideoCapture capture(parser.get<String>("input"));
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open: " << parser.get<String>("input") << endl;
        return 0;
    }
    //! [capture]



    Mat frame, fgMask;
    while (true) {
        capture >> frame;
        if (frame.empty())
            break;

        //! [apply]
        //update the background model
        auto timeNow = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = timeNow - timeStart;
        if(elapsed_seconds.count() <= 3.0){
            pBackSub->apply(frame, fgMask, -1);
            std::cout <<"Updating Background\n";
        } else {
            pBackSub->apply(frame, fgMask, 0.001); //0 -> Do not update model
            std::cout <<"NOT UPDATING BACKGROUND\n";
        }
        //! [apply]

        //! [display_frame_number]
        //get the frame number and write it on the current frame
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        stringstream ss;
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //! [display_frame_number]

        //! [bounding_box]
        cv::Mat frameThresh;
        cv::threshold(fgMask,frameThresh,200,255,cv::THRESH_BINARY);

        cv::Mat element = getStructuringElement( cv::MORPH_ERODE,
                             Size( 3, 3 ),
                             Point( 2, 2 ) );
        cv::Mat frameErode;
        cv::erode(frameThresh,frameErode,element);

        //Find Image Contours
        vector<vector<Point> > contours;
        cv::findContours(frameErode,contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));

        //Sort Contours
        std::sort(contours.begin(),contours.end(), [](vector<Point> a, vector<Point> b){
            return contourArea(a) > contourArea(b);
        });
        std::vector<std::vector<cv::Point>>::iterator itr;
        int contAreaThresh = 100;
        double contArea = 0.0;
        std::vector<std::vector<cv::Point>> contourKeep;
        for(itr = contours.begin(); itr !=contours.end(); itr++){
            contArea = contourArea(*itr);
            std::cout << contArea << "\n";
            if(contArea > contAreaThresh){
                contourKeep.push_back(*itr);
            }
        }
        contours = contourKeep;


        vector<RotatedRect> minRect( contours.size() );
        for( size_t i = 0; i < contours.size(); i++ )
        {
            minRect[i] = minAreaRect( contours[i] );
        }
        // rotated rectangle
        for( size_t i = 0; i< contours.size(); i++ )
        {
            Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
            // contour
            drawContours( frame, contours, (int)i, color );
            // rotated rectangle
            Point2f rect_points[4];
            minRect[i].points( rect_points );
            for ( int j = 0; j < 4; j++ )
            {
                line( frame, rect_points[j], rect_points[(j+1)%4], color );
            }
        }
        //! [bounding_box]

        //! [show]
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask", fgMask);
        cv::imshow("Erode",frameErode);
        //! [show]

        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }

    return 0;
}
