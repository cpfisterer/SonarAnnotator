#ifndef SONARDATA_H
#define SONARDATA_H

/*
 *  SonarData.h
 *  iSonar
 *
 *  Created by Carl Pfisterer on 6/4/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */
#define QT_USE_FAST_CONCATENATION

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "pinglist.h"
#include "imageprocessing.h"
#include <QDateTime>                    //Used for convience since Simrad date and time is saved using a single 64 bit integer
#include <QDebug>
#include <QProgressDialog>
#include <QtGlobal>
#include <QtEndian>
#include <QObject>
#include <QString>

class SonarData : public QObject{
    Q_OBJECT
private:
    // Variables
protected:
    // Variables
    unsigned short *amplitude;                              //Amplitude is the strength in dB adjusted for SL, Gain, TVG, but not angle multiplied by -500.
                                                            //This allows values as small as -120dB to 3 decimal places.
    signed short *horizontal, *vertical;                    //Angles are in degrees multiplied by 1000 to give +-32 deg with 3 decimal places
    PingList *pingArray;
    char *manufacturer;

    //	Calibration values
    float SL,G1,Gr,alpha,tvg,threshold,c;
    float hBeamWidth,vBeamWidth;                            //Beam width (field of view)
    float *TVGValues;
    float *backgroundValues;
    float *stdDevValues;

    float maxRange,minRange,pingRate,samplesPerMeter;
    int numberOfPings,numberOfSamples, numberToAverage;
    bool rotate,flip;

    FILE* file;

    QDateTime startTime,endTime;            //Note the QDateTime is specific to Qt!
    QProgressDialog *progress;

    //Member functions
public:
    SonarData();
    virtual ~SonarData();
    virtual void calculateBackground();
    void exportData(int start, int end, char *saveName);
    virtual void exportHeader(FILE *saveFile, int number);
    void exportPings(int start,int end, char *saveName);
    static short fromLittleEndian(short value);
    static int fromLittleEndian(int value);
    static long long fromLittleEndian(long long value);
    static float fromLittleEndian(float value);
    static double fromLittleEndian(double value);
    float * getBackground();
    virtual float getCompass(){return 0.0;}
    virtual QString getConfiguration(){return QString("NA");}
    float getDecibel(int sample, int ping);
    float* getDecibels(Rectangle rect,float minMarkRange,float maxMarkRange);
    QDateTime getEndTime();
    virtual float getFocus(){return 0.0;}
    virtual float getGain(){return Gr;}
    virtual float getHumidity(){return 0.0;}
    float* getHorizontal(Rectangle rect);
    float getHorizontalAngle(int sample,int ping);
    float getHorizontalBeamWidth();
    virtual float getLength(){return (maxRange-minRange);}
    char* getManufacturer();
    float getMaxRange();
    float getMinRange();
    int getNumberOfPings();
    int getNumberOfSamples();
    float getPingRate();
    virtual float getPitch(){return 0.0;}
    virtual QString getPower(){return QString("NA");}
    float getRange(int sample);
    virtual float getRoll(){return 0.0;}
    float getSamplesPerMeter();
    virtual float getStartRange(){return minRange;}
    QDateTime getStartTime();
    virtual QString getSerialNumber(){return QString("");}
    float *getStdDevValues();
    virtual float getTemperature(){return 0.0;}
    float getTime(int ping);
    float* getVertical(Rectangle rect);
    float getVerticalAngle(int sample,int ping);
    float getVerticalBeamWidth();
    float getVoltage(int sample, int ping);
    float* getVoltages(Rectangle rect);
    virtual bool openFile(char *filename);
    float* readAmplitude(int ping);
    static void readBuffer(FILE* file,char* buffer,int length);
    static char readChar(FILE* file);
    static double readDouble(FILE* file);
    static float readFloat(FILE* file);
    static int readInt(FILE* file);
    static long long readLong(FILE* file);
    static short readShort(FILE* file); 
    virtual void setAlpha(float value);
    void setFlip(bool doFlip);
    void setNumberToAverage(int value);
    void setRotate(bool doRotate);
    virtual void setTVG(float value);

protected:
    Rectangle checkRectangle(Rectangle rect);
    virtual void clearData();
    void closeFile();
    float getAngle(signed short value);
    float getDecibel(unsigned short value);
    float getVoltage(float value);
    virtual PingList* indexFile(FILE* file);
    virtual void readConfiguration(FILE* file);    
    virtual void readData(FILE* file); 
    void setTVGValues();
    virtual float TVGCompensation(int sample);

signals:
    void appendText(QString newText);
};

#endif // SONARDATA_H
