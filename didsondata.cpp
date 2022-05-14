/*
 *  DIDSONData.cpp
 *  iSonar
 *
 *  Created by Carl Pfisterer on 6/27/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include "didsondata.h"

DIDSONData::DIDSONData():SonarData()
{
    strcpy(manufacturer,"SMC/DIDSON");
    frame = nullptr;
    videoBackground = nullptr;
    bigLens = false;
    reverse = false;
    useMaximumBeam = false;
    numberOfBeamsToAverage = 4;
    didsonCorrection = -100;
}

DIDSONData::~DIDSONData(){
    if(frame){
        delete frame;
    }
    qDebug() << "Didson Data deallocated";
}

void DIDSONData::clearData(){
    if(videoBackground){
        delete [] videoBackground;
        videoBackground = nullptr;
    }
    SonarData::clearData();
}

void DIDSONData::exportHeader(FILE *saveFile,int number){
    char *buffer;

    buffer = new char[fileHeaderLength];
    fread(buffer,sizeof(char),size_t(fileHeaderLength),file);
    fwrite(buffer,sizeof(char),size_t(fileHeaderLength),saveFile);
    fseeko(saveFile,4,SEEK_SET);
    fwrite(&number,sizeof(int),1,saveFile);         //  ** Note, this is not big endian safe/compatible
    fseeko(saveFile,fileHeaderLength,SEEK_SET);

    delete [] buffer;
}

float *DIDSONData::getBackgroundFrame(int frameNumber){
    float *data = new float[numberOfSamples*numberOfBeams];
    int i,j,index;

    for(i = 0; i < numberOfSamples; i++){
        for(j = 0; j < numberOfBeams; j++){
            index = int(floor(frameNumber/(numberOfPings/10)));
            data[i*numberOfBeams+j] = videoBackground[index*numberOfSamples*numberOfBeams+i*numberOfBeams+j];
        }
    }
    return data;
}

float DIDSONData::getCompass(){
    return frame->getPan();
}

QString DIDSONData::getConfiguration(){
    QString useTelephoto;
    if(bigLens) useTelephoto = "True";
    else useTelephoto = "False";
    QString resolution;
    if(hResolution == 0){
        resolution = "High Frequency";
    }
    else{
        resolution = "Low Frequency";
    }
    QString config = tr("Manufacturer/Model: %1\nCollection Date and Time: %2\nFile Duration: %3 min\nStart Range: %4\nEnd Range: %5\nWindow Length: %6\nNumber of Beams: %7\nSamples Per Beam: %8\nNumber of Frames: %9\nTelephoto Lens: %10\nVersion: %11\nResolution: %12\nSystem Type: %13").
            arg(this->getManufacturer()).
            arg(frame->getDateTimeString()).
            arg(double(startTime.secsTo(endTime)/60.0),0,'f',2).
            arg(double(frame->getWindowStart()),0,'f',2).
            arg(double(frame->getWindowStart()+frame->getWindowLength()),0,'f',2).
            arg(double(frame->getWindowLength()),0,'f',2).
            arg(numberOfBeams).
            arg(numberOfSamples).
            arg(numberOfPings).
            arg(useTelephoto).
            arg(version).
            arg(resolution).
            arg(frame->getSystemType());
    return config;
}

float DIDSONData::getFocus(){
    float focus = frame->getFocus();
    focus = float(frame->getWindowLength()/numberOfSamples) * focus;
    return focus;
}

DIDSONFrame * DIDSONData::getFrame(){
    return frame;
}

float DIDSONData::getGain(){
    return frame->getGain();
}

float DIDSONData::getHumidity(){
    return frame->getHumidity();
}

float DIDSONData::getIndividualBeamWidth(){
    float indivWidth = float(0.6);
    if(numberOfBeams==96){
        indivWidth = indivWidth/float(2.0);
    }
    else if(numberOfBeams==128){
        indivWidth = indivWidth/float(2.6667);
    }
    if(bigLens==true){
        indivWidth = indivWidth/float(2.0);
    }
    return indivWidth;
}

float DIDSONData::getLength(){
    return frame->getWindowLength();
}

int DIDSONData::getNumberOfBeamsToAverage(){
    return numberOfBeamsToAverage;
}

float DIDSONData::getPitch(){
    return frame->getPitch();
}

QString DIDSONData::getPower(){
    return tr("%1 V").arg(double(frame->getPower())/10);
}

float DIDSONData::getRoll(){
    return frame->getRoll();
}

bool DIDSONData::getReverse(){
    return reverse;
}

QString DIDSONData::getSerialNumber(){
    if(frame){
        return tr("%1").arg(frame->getSerialNumber());
    }
    else{
        return QString("No frame available");
    }
}

float DIDSONData::getStartRange(){
    return frame->getWindowStart();
}

float DIDSONData::getTemperature(){
    return frame->getTemperature();
}

float *DIDSONData::getVideoBackground(){
    return videoBackground;
}

PingList *DIDSONData::indexFile(FILE *newFile){
    PingList *list = new PingList;
    Ping* p;
    int numPings = 0,i;
    int frameLength = numberOfBeams*numberOfSamples+frameHeaderLength;
    long int startOffset=0;

    startOffset= ftello(newFile);
    fseeko(newFile,4,SEEK_SET);
    numPings=readInt(newFile);
    for(i = 0; i < numPings; i++){
        p = new Ping;
        p->numberOfSamples = numberOfSamples;
        p->pingNumber = i;
        p->fileLocation = fileHeaderLength+i*frameLength;
        list->add(p);
    }
    fseeko(newFile,startOffset,SEEK_SET);

    return list;
}

bool DIDSONData::isBigLens(){
    return bigLens;
}

void DIDSONData::readConfiguration(FILE *newFile){
    char buffer[5];

    this->readBuffer(newFile,buffer,3);buffer[3] = '\0';
    version = int(this->readChar(newFile));
    emit appendText(tr("File Version: %1%2").arg(buffer).arg(version));
    if(version == 3){
        fileHeaderLength = 512;
        frameHeaderLength = 256;
        this->readConfigurationV3(newFile);
    }
    else if (version == 4){
        fileHeaderLength = 1024;
        frameHeaderLength = 1024;
        this->readConfigurationV4(newFile);
    }
    else {}//Error
}

void DIDSONData::readConfigurationV3(FILE *newFile){
    float windowLengths[]={1.125f,2.25f,4.5f,9.0f,19.0f,36.0f};
    int intValue;
    float floatValue;

    numberOfPings = readInt(newFile); emit appendText(tr("Number of Frames: %1").arg(numberOfPings));	//	Number of pings
    pingRate = readInt(newFile); emit appendText(tr("Frame Rate: %1").arg(double(pingRate)));                   //	Ping rate
    hResolution = readInt(newFile);                                                                     //	Horizontal resolution flag
    if(hResolution == 0) {hResolution = 1;emit appendText(tr("Low Resolution"));}
    else {hResolution = 0;emit appendText(tr("High Resolution"));}
    numberOfBeams = readInt(newFile); emit appendText(tr("Number of Beams: %1").arg(numberOfBeams));   //      Number of beams
    floatValue = readFloat(newFile);emit appendText(tr("Sample Rate: %1").arg(double(floatValue)));                  //       Sample rate
    numberOfSamples = readInt(newFile);                                                                //	Samples per channel (512 for now)
    intValue = readInt(newFile);                                                                       //	Gain
    Gr = float(intValue); emit appendText(tr("Gain: %1").arg(double(Gr)));
    intValue = readInt(newFile);                                                                       //	Start range
    minRange = float(intValue)*(0.375f+float(hResolution)*0.375f);
    intValue = readInt(newFile);                                                                       //	Window length flag
    maxRange = windowLengths[intValue]+minRange;
    intValue = readInt(newFile);                                                                       //	Reverse
    if(intValue == 1){reverse = true;emit appendText(tr("Reverse: True"));}
    else{reverse = false;emit appendText(tr("Reverse: False"));}
    serialNumber=readInt(newFile);	appendText(tr("Serial Number: %1").arg(serialNumber));          	//	Serial number
    fseeko(newFile,32,SEEK_CUR);                                                                        //	Date
    fseeko(newFile,256,SEEK_CUR);                                                                       //	User defined
    intValue = readInt(newFile);intValue = readInt(newFile);
    intValue = readInt(newFile);intValue = readInt(newFile);
    intValue = readInt(newFile);                                                                       //	Start frame
    intValue = readInt(newFile);                                                                       //	End frame
    intValue = readInt(newFile);                                                                       //	Elapsed time
    intValue = readInt(newFile);                                                                       //	Record interval
    intValue = readInt(newFile);                                                                       //	Radio sec.
    intValue = readInt(newFile);                                                                       //	Frame interval
    m_nFlags = uint(readInt(newFile));                                                                       //  Collection flags including big lens
    intValue = readInt(newFile);                                                                       //  Aux information
    c = readInt(newFile); emit appendText(tr("Speed of Sound: %1").arg(double(c)));                            //  Speed of sound
    fseeko(newFile,124,SEEK_CUR);                                                                       //	User
    frameLength = numberOfSamples*numberOfBeams;
    samplesPerMeter = numberOfSamples/(maxRange-minRange);
    if(numberOfBeams<96) hBeamWidth = float(0.6 * numberOfBeams);                                          // phi is the degrees per beam
    else hBeamWidth = float(0.3 * numberOfBeams);
    if((m_nFlags&0x00000f00) == 512 || (m_nFlags&0x00000f00) == 768){
        hBeamWidth = hBeamWidth/2;  //If big lens, divide beam width by 2
        bigLens = true;
    }
    if(frame){delete frame;}
    frame = new DIDSONFrame(newFile,numberOfBeams,hResolution,version);
    numberOfBeamsToAverage = int(fmin(numberOfBeamsToAverage,numberOfBeams));        //Ensure we don't average across more beams than present
    frame->setAvgBeams(numberOfBeamsToAverage);
    frame->setDecibelCorrection(didsonCorrection);
}

void DIDSONData::readConfigurationV4(FILE *newFile){
    DIDSONData::readConfigurationV3(newFile);
    fseeko(newFile,512,SEEK_CUR);                           //Current difference is the extra padding at the end
}

void DIDSONData::readData(__attribute__((unused)) FILE *newFile){
    float *centerBeam,*angles,tempFloat,*frameData;
    int i,j,k,index;

    amplitude = new unsigned short[numberOfSamples*numberOfPings];                      //Reallocate data buffers
    horizontal = new signed short[numberOfSamples*numberOfPings];
    vertical = new signed short[numberOfSamples*numberOfPings];
    videoBackground = new float[numberOfSamples*numberOfBeams*10];  //Memory buffer to contain 10 background frame images
    this->intializeBackground();
    if(!amplitude || !horizontal || !vertical || !videoBackground){
        qDebug() << "Problem allocating memory";
    }
    for(i=0; i < numberOfPings; i++){
        progress->setValue(i);
        if(i == 0){
            frame->goToFrame(0);
            frame->setFirstFrame();
        }
        else frame->readFrameNoTVG();
        if(!useMaximumBeam){
            centerBeam = frame->centerBeam();
        }
        else{
            centerBeam = frame->maximumBeam();
        }
        frameData = frame->getFrame();
        angles = frame->angles();
        for(j = 0; j < numberOfSamples; j++){
            amplitude[i*numberOfSamples+j] = centerBeam[j];
            horizontal[i*numberOfSamples+j] = short(1000*angles[j]);
            vertical[i*numberOfSamples+j] = 0;
            for(k = 0; k < numberOfBeams; k++){
                index = int(fmin(9,floor(i/(numberOfPings/10))));       //Determines which of the 10 background frames to use
                videoBackground[index*numberOfBeams*numberOfSamples+j*numberOfBeams+k] += frameData[j*numberOfBeams+k];
            }
        }
        delete [] centerBeam;
        delete [] angles;
    }
    readTimes();
    pingRate = float(numberOfPings)/float(endTime.toTime_t()-startTime.toTime_t());   //Adjust ping rate to reflect actual time sampled
    frame->goToFrame(0);
    maxRange = frame->getMaximumRange();                                                //Obtain min & max ranges and samples per meter from 1st frame
    minRange = frame->getWindowStart();                                                 //rather than from file header.
    samplesPerMeter = numberOfSamples/(maxRange-minRange);
    for(i = 0; i < numberOfSamples*numberOfBeams*10; i++){
        videoBackground[i] = videoBackground[i]/(numberOfPings/10);
    }
}

void DIDSONData::intializeBackground(){
    int i;

    for(i = 0; i < numberOfSamples*numberOfBeams*10;i++){
        videoBackground[i] = 0;
    }
}

void DIDSONData::readTimes(){
    frame->goToFrame(0);
    startTime = frame->getDateTime();
    frame->goToFrame(numberOfPings-1);
    endTime = frame->getDateTime();
}

void DIDSONData::setAlpha(float value){
    SonarData::setAlpha(value);
    if(frame){
        frame->setAlpha(value);
    }
}

void DIDSONData::setDidsonCorrection(float value){
    didsonCorrection = value;
    if(frame){
        frame->setDecibelCorrection(didsonCorrection);
    }
}

void DIDSONData::setNumberOfBeamsToAverage(int value){
    numberOfBeamsToAverage = value;
}

void DIDSONData::setTVG(float value){
    SonarData::setTVG(value);
    if(frame){
        frame->setTVG(value);
    }
}

void DIDSONData::setUseMaximumBeam(bool value){
    useMaximumBeam = value;
}
