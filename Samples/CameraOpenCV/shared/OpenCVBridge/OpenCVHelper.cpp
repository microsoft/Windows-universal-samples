//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
// OpenCVHelper.cpp

#include "pch.h"
#include "OpenCVHelper.h"
#include "MemoryBuffer.h"
#include <iostream>
using namespace Microsoft::WRL;

using namespace OpenCVBridge;
using namespace Platform;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

using namespace cv;

OpenCVHelper::OpenCVHelper()
{
    pMOG2 = createBackgroundSubtractorMOG2();
}

void OpenCVHelper::Blur(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
    Mat inputMat, outputMat;
    if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
    {
        return;
    }
    
    blur(inputMat, outputMat, cv::Size(5, 5));
}

void OpenCVHelper::MotionDetector(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
    Mat inputMat, outputMat;
    if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
    {
        return;
    }
    
    pMOG2->apply(inputMat, fgMaskMOG2);
    int type = fgMaskMOG2.type();
    Mat temp;
    cvtColor(fgMaskMOG2, temp, CV_GRAY2BGRA);

    Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
    erode(temp, temp, element);
    temp.copyTo(outputMat);
}

void OpenCVHelper::Histogram(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
    Mat inputMat, outputMat;
    if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
    {
        return;
    }

    std::vector<Mat> bgr_planes;
    split(inputMat, bgr_planes);
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;

    Mat b_hist, g_hist, r_hist;
    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
    int hist_w = outputMat.cols; int hist_h = outputMat.rows;
    double bin_w = (double)outputMat.cols / histSize;

    normalize(b_hist, b_hist, 0, outputMat.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, outputMat.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, outputMat.rows, NORM_MINMAX, -1, Mat());
    for (int i = 1; i < histSize; i++)
    {
        int x1 = cvRound(bin_w * (i - 1));
        int x2 = cvRound(bin_w * i);
        line(outputMat, cv::Point(x1, hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(x2, hist_h - cvRound(b_hist.at<float>(i))),
            Scalar(255, 0, 0, 255), 2, 8, 0);
        line(outputMat, cv::Point(x1, hist_h - cvRound(g_hist.at<float>(i - 1))),
            cv::Point(x2, hist_h - cvRound(g_hist.at<float>(i))),
            Scalar(0, 255, 0, 255), 2, 8, 0);
        line(outputMat, cv::Point(x1, hist_h - cvRound(r_hist.at<float>(i - 1))),
            cv::Point(x2, hist_h - cvRound(r_hist.at<float>(i))),
            Scalar(0, 0, 255, 255), 2, 8, 0);
    }
}

void OpenCVHelper::Contours(SoftwareBitmap^ input, SoftwareBitmap^ output) 
{
    Mat inputMat, outputMat;
    if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
    {
        return;
    }

    Mat src_gray;
    Mat canny_output;
    int thresh = 50;
    int max_thresh = 255;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<Vec4i> hierarchy;

    cvtColor(inputMat, src_gray, CV_BGRA2GRAY);
    blur(src_gray, src_gray, cv::Size(3, 3));
    Canny(src_gray, canny_output, thresh, thresh * 3, 3);
    findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (int i = 0; i < contours.size(); i++)
    {
        drawContours(outputMat, contours, i, Scalar(255, 0, 0, 255), 2, 8, hierarchy, 0);
    }
}

void OpenCVHelper::HoughLines(SoftwareBitmap^ input, SoftwareBitmap^ output) 
{
    Mat inputMat, outputMat;
    if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
    {
        return;
    }

    inputMat.copyTo(outputMat);

    Mat edges, cdst, src_gray;
    cvtColor(inputMat, src_gray, CV_BGRA2GRAY);
    Canny(src_gray, edges, 100, 200, 3);
    std::vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI / 180, 50, input->PixelWidth / 4, 10);
    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i l = lines[i];
        line(outputMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), Scalar(0, 255, 0, 255), 3, CV_AA);
    }
}

bool OpenCVHelper::TryConvert(SoftwareBitmap^ from, Mat& convertedMat)
{
    unsigned char* pPixels = nullptr;
    unsigned int capacity = 0;
    if (!GetPointerToPixelData(from, &pPixels, &capacity))
    {
        return false;
    }

    Mat mat(from->PixelHeight,
        from->PixelWidth,
        CV_8UC4, // assume input SoftwareBitmap is BGRA8
        (void*)pPixels);

    // shallow copy because we want convertedMat.data = pPixels
    // don't use .copyTo or .clone
    convertedMat = mat;
    return true;
}

bool OpenCVHelper::GetPointerToPixelData(SoftwareBitmap^ bitmap, unsigned char** pPixelData, unsigned int* capacity)
{
    BitmapBuffer^ bmpBuffer = bitmap->LockBuffer(BitmapBufferAccessMode::ReadWrite);
    IMemoryBufferReference^ reference = bmpBuffer->CreateReference();

    ComPtr<IMemoryBufferByteAccess> pBufferByteAccess;
    if ((reinterpret_cast<IInspectable*>(reference)->QueryInterface(IID_PPV_ARGS(&pBufferByteAccess))) != S_OK)
    {
        return false;
    }

    if (pBufferByteAccess->GetBuffer(pPixelData, capacity) != S_OK)
    {
        return false;
    }
    return true;
}