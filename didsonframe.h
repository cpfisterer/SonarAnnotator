#ifndef DIDSONFRAME_H
#define DIDSONFRAME_H

/*
 *  DIDSONFrame.h
 *  Echotastic2
 *
 *  Created by Carl Pfisterer on 10/2/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */
#include <QDateTime>
#include <QDebug>
#include <QByteArray>
#include "sonardata.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>


class DIDSONFrame{
protected:
    float *frame;
    int numberOfBeams,gain,threshold,intensity,serialNumber,resolution,version,firstFrame,configFlag;
    int fileHeaderLength,frameLength,frameHeaderLength,currentFrameNumber,avgBeams;
    int year,month,day,hour,minute,second;
    float windowStart,windowLength,alpha,*tvgValues,tvg;
    float decibelCorrection;
    float pan,tilt,roll,focus,power,temperature,humidity;
    QDateTime frameTime;
    FILE *file;
    int numberOfSamples;
public:
    DIDSONFrame();
    DIDSONFrame(FILE *newFile,int numBeams,int res,int ver);
    virtual ~DIDSONFrame();
    virtual void readFrame();
    virtual void readFrameNoTVG();
    virtual void goToFrame(int frameNumber);
    float * centerBeam();
    float * maximumBeam();
    float * angles();
    float getFocus(){return focus;}
    float *getFrame();
    float getGain(){return float(gain);}
    float getHumidity(){return humidity;}
    float getPan(){return pan;}
    float getPitch(){return tilt;}
    float getPower(){return power;}
    float getRoll(){return roll;}
    virtual QString getSystemType();
    float getTemperature(){return temperature;}
    float getWindowStart();
    float getWindowLength();
    float getMaximumRange();
    float getNumberOfBeams();
    float getNumberOfSamples();
    int getSerialNumber(){return serialNumber;}
    int getFrameNumber();
    char * getDateTimeString();
    QDateTime getDateTime();
    void setAvgBeams(int value);
    void setDecibelCorrection(float value);
    void setFirstFrame();
    void setTVG(float value);
    void setAlpha(float value);
    void correctTVG(float *data);

protected:
    virtual void readFrameHeader();
    void calculateTVGValues();
    void correctTVG(unsigned char *data);
    void correctAmplitude(unsigned char *data);
};

#endif // DIDSONFRAME_H
