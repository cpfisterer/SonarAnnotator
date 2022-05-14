#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

/*
 *  ImageProcessing.h
 *  iSonar
 *
 *  Created by Carl Pfisterer on 7/1/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include <QDebug>
#include <math.h>
#include <stdio.h>

typedef float pixelvalue;
//typedef unsigned char pixelvalue;
#define PIX_SORT(a,b) {if((a)>(b)) PIX_SWAP((a),(b));}
#define PIX_SWAP(a,b) {pixelvalue temp=(a);(a)=(b);(b)=temp;}
//#define BRACKET(a,b,c) (((a)<(b)) ? 0:((a)>(c)) ? (c):(a))

inline
        int bracket(int a,int b,int c){
    return (((a)<(b)) ? 0:((a)>(c)) ? (c):(a));
}

inline
        int min(int x,int y){
    return (x>y) ? y:x;
}

inline
        int max(int x,int y){
    return (x>y) ? x:y;
}

pixelvalue opt_med3(pixelvalue *p);
pixelvalue opt_med4(pixelvalue *p);
pixelvalue opt_med6(pixelvalue *p);
pixelvalue opt_med9(pixelvalue * p);
pixelvalue opt_med25(pixelvalue * p);

struct Rectangle{
        int x,y,w,h;
};

struct Color{
        unsigned char red,green,blue;
};

unsigned char* convolve3x3(unsigned char* inValues, float *kernel, int w, int h);
unsigned char* lowPassFilter3x3(unsigned char* inValues, int w, int h);
unsigned char* medianFilter3x3(unsigned char* inValues, int w, int h);
unsigned char* medianFilter5x5(unsigned char* inValues, int w, int h);
unsigned char* adjustContrast(unsigned char* values, int w, int h, float contrast);
unsigned char* adjustGamma(unsigned char* values, int w, int h, float gamma);
unsigned char* adjustContrastGamma(unsigned char* values, int w, int h, float contrast, float gamma);
float* averageImage(float *frame1, float *frame2, float *frame3, int width, int height);
float* medianImage(float *frame1, float *frame2, float *frame3, int w, int h);
unsigned char* mapDecibelToChar(float* amplitude, int w, int h,float lowerThreshold, float upperThreshold);
unsigned char* mapDecibelToCharAdjustContrastGamma(float* amplitude, int w, int h,float lowerThreshold, float upperThreshold,float contrast,float gamma);
unsigned char* mapVoltageToChar(float* amplitude, int w, int h, float maximum, float minimum);
unsigned char* mapVoltageToCharAdjustContrastGamma(float* amplitude, int w, int h, float maximum, float minimum,float contrast,float gamma);
unsigned char* mapAngleToChar(float* angle, int w, int h, float limit);
unsigned char* mapValuesToColor(unsigned char* amplitude, Color* map, int w, int h);
float* removeBackground(float* amplitude, float* background, Rectangle r, Rectangle r2, float cutoff, bool rotate);
float* removeBackground(float* amplitude, float* background, float* se, Rectangle r, Rectangle r2,float cutoff, bool rotate);
float* removeBackground(float* amplitude, float* background, int frameNumber, int totalFrames,int width, int height, float cutoff);
float* duplicateImage(float* data, int width, int height);
float* subtractBackground(float *amplitude, float* background, Rectangle r, Rectangle r2, bool rotate);
float* subtractBackground(float *amplitude, float* background,int frameNumber, int totalFrames,int width, int height);

class ImageFilter{
private:

protected:

public:
    virtual float* process(float* data, Rectangle r);
    virtual ~ImageFilter() = 0;
};

class AverageImage:public ImageFilter{
private:
    using ImageFilter::process;
protected:

public:
    float* process(float* data, float* data2, float* data3, Rectangle r);
};

class ContrastGammaFilter:public ImageFilter{
private:
    float contrast,gamma;
protected:

public:
    void setContrast(float value);
    void setGamma(float value);
    float* process(float* data, Rectangle r);
};

class ConvolveFilter:public ImageFilter{
private:
    float* kernel;
protected:

public:
    ConvolveFilter();
    ~ConvolveFilter();
    void setKernel(float* values);
    float* process(float* data, Rectangle r);
};

class CorrectTVGFilter:public ImageFilter{
private:
    using ImageFilter::process;
protected:

public:
    float* process(float* data, float start, float mps);
};

class LowPassFilter:public ImageFilter{
private:
    float* kernel;
protected:

public:
    LowPassFilter();
    ~LowPassFilter();
    float* process(float* data, Rectangle r);
};

class MapAngleToChar:public ImageFilter{
private:
    float limit;
protected:

public:
    void setLimit(float value);
    float* process(float* data, Rectangle r);
};

class MapDecibelToChar:public ImageFilter{
private:
    float lower,upper,contrast,gamma;
protected:

public:
    void setLower(float value);
    void setUpper(float value);
    void setContrastGamma(float value1,float value2);
    float* process(float* data, Rectangle r);
};

class MapVoltageToChar:public ImageFilter{
private:
    float lower,upper,contrast,gamma;
protected:

public:
    void setLower(float value);
    void setUpper(float value);
    void setContrastGamma(float value1,float value2);
    float* process(float* data, Rectangle r);
};

class MedianFilter:public ImageFilter{
private:

protected:

public:
    float* process(float* data, Rectangle r);
};

class RemoveBackground:public ImageFilter{
private:
    float cutoff;
    bool rotate;
    using ImageFilter::process;
protected:

public:
    void setCutoff(float value);
    void setRotate(bool value);
    float* process(float* data, float* background, float* sdDev,Rectangle r, Rectangle r2);
    float* process(float *data, float *background, int frameNumber, int totalFrames, Rectangle r);
};


#endif // IMAGEPROCESSING_H
