#ifndef ARISFRAME_H
#define ARISFRAME_H

#include <QDateTime>
#include <QDebug>
#include "sonardata.h"
#include "didsonframe.h"

class ArisFrame: public DIDSONFrame{

    int previousNumberOfSamples,previousNumberOfBeams,pingMode,frequencyHiLo,pulseWidth,cyclePeriod;
    int samplePeriod,sampleStartDelay,largeLens,systemType,appVersion,reorderedSamples,salinity;
    float sampleRate,c,pressure;

public:
    ArisFrame(FILE *newFile,int numBeams,int res, int ver);
    void readFrame();
    void readFrameNoTVG();
    void goToFrame(int frameNumber);
    float getSpeedOfSound();
    bool getLargeLens();
    QString getSystemType();

protected:
    void readFrameHeader();
};

#endif // ARISFRAME_H
