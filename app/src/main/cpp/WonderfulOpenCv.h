//
// Created by Acer on 2020/3/15.
//

#ifndef WYOPENCV_WONDERFULOPENCV_H
#define WYOPENCV_WONDERFULOPENCV_H

#include <opencv2/imgproc.hpp>
#include "CascadeDetectorAdapter.h"
#include "androidlog.h"
using namespace cv;

class WonderfulOpenCv {

public:
    WonderfulOpenCv(const char *faceModelPath);
    ~WonderfulOpenCv();
    uint8_t* transformGray(uint8_t *rgb,int width,int height);              //rgb转灰度图
    Mat rgb2Mat(uint8_t *rgb,int width,int height);                         //rgb转Mat
    uint8_t*  mat2rgb(Mat mat, int colorType);                              //Mat转rgb
    uint8_t* nv21ToRgb(uint8_t *nv21,int width,int height);                  //NV21转rgb
    uint8_t* rgb2rgba(uint8_t *data, int width, int height, uint8_t alpha); //rgb转rgba
    void dynamicFaceCheck(Mat &frame);                                      //动态人脸检测
    void initDetectorTracker();                                             //初始化追踪器

private:
    char *faceModelPath = nullptr;                        //人脸模型路径
//    DetectionBasedTracker *detectorTracker = nullptr;     //追踪器
    Ptr<DetectionBasedTracker> detectorTracker = nullptr; //追踪器
    int tracking = 0;                                     //追踪器运行状态
};

#endif //WYOPENCV_WONDERFULOPENCV_H
