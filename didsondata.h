#ifndef DIDSONDATA_H
#define DIDSONDATA_H

/*
 *  DIDSONData.h
 *  Echotastic
 *
 *  Created by Carl Pfisterer on 6/27/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include "sonardata.h"
#include "didsonframe.h"
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QProgressDialog>

class DIDSONData: public SonarData {
    Q_OBJECT
protected:
    int numberOfBeams,fileHeaderLength,frameHeaderLength,frameLength,serialNumber,hResolution;
    int frameNumber;    //The current frame number
    int numberOfBeamsToAverage;
    int version;
    unsigned int m_nFlags;
    bool reverse;
    bool useMaximumBeam;
    bool bigLens;
    DIDSONFrame *frame;
    float didsonCorrection;
    float *videoBackground;

public:
    DIDSONData();
    ~DIDSONData();
    void exportHeader(FILE *saveFile,int number);
    float getCompass();
    float *getBackgroundFrame(int number);
    QString getConfiguration();
    float getFocus();
    DIDSONFrame *getFrame();
    float getGain();
    float getHumidity();
    float getIndividualBeamWidth();
    float getLength();
    int getNumberOfBeamsToAverage();
    float getPitch();
    QString getPower();
    bool getReverse();
    float getRoll();
    QString getSerialNumber();
    float getStartRange();
    float getTemperature();
    float *getVideoBackground();
    bool isBigLens();
    void setNumberOfBeamsToAverage(int value);
    void setUseMaximumBeam(bool value);
    void setDidsonCorrection(float value);
    void setAlpha(float value);
    void setTVG(float value);
protected:
    void clearData();
    PingList *indexFile(FILE *newFile);
    void intializeBackground();
    void readConfiguration(FILE *newFile);
    void readConfigurationV3(FILE *newFile);
    void readConfigurationV4(FILE *newFile);
    void readData(FILE *newFile);
    void readTimes();
};

#endif // DIDSONDATA_H
