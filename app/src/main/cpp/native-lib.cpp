#include <jni.h>
#include <string>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <time.h>

JNIEXPORT void JNICALL
Java_org_opencv_aruco_Aruco_detectMarkers_11(JNIEnv *env, jclass type, jlong image_nativeObj, jlong dictionary_nativeObj, jlong corners_mat_nativeObj, jlong ids_nativeObj) {

    // TODO

}
extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_detectMarkers(JNIEnv *env, jobject instance, jlong image_nativeObj) {

    cv::Mat& inMat = *(cv::Mat*)image_nativeObj;

    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;

    //cv::resize(inMat, inMat, cv::Size(640, 480), 0, 0, cv::INTER_CUBIC);

    //cv::GaussianBlur(inMat, inMat, cv::Size(3, 3), 1.5, 1.5);

    cv::cvtColor(inMat,inMat,CV_RGB2GRAY);

    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    cv::aruco::detectMarkers(inMat, cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250), corners, ids);

    cv::aruco::drawDetectedMarkers(inMat,corners,ids);

}

extern "C"
jstring
Java_com_example_raven_pathfindar_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {

    cv::Mat test;
    cv::VideoCapture camera;
    camera.open(0);
    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::Mat marker;
    cv::aruco::drawMarker(dict, 25, 200, marker, 1);
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
jstring
Java_com_example_raven_pathfindar_MainActivity_detectMarker(JNIEnv *env, jobject /* this */) {

    cv::Mat test;
    cv::VideoCapture camera;
    camera.open(0);
    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::Mat marker;
    cv::aruco::drawMarker(dict, 25, 200, marker, 1);
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
jstring
Java_com_example_raven_pathfindar_MainActivity_secondString(
        JNIEnv *env,
jobject /* this */) {
    std::string hello = "Hello from C++ 2";
    return env->NewStringUTF(hello.c_str());
}


