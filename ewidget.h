#ifndef EWIDGET_H
#define EWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QObject>
#include <QtGlobal>
#include <QImage>
#include<QModelIndexList>
#include <QString>
#include <QList>
#include <QPixmap>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>
#include "sonardata.h"
#include "didsondata.h"
#include "imageprocessing.h"

class EWidget : public QWidget
{
    Q_OBJECT
    SonarData *sonarData;
    Color *colorMap;
    QColor backgroundColor,downstreamColor,upstreamColor;
    bool rotate,flip;
    bool filterBackground,drawing;
    bool doLowpass, doMedian, doConvolve, hasChanged, dragging;
    float timeScaler,rangeScaler;
    float lowerThreshold,upperThreshold,gammaValue,contrastValue,maxVoltage;
    float minMarkRange,maxMarkRange;
    QImage *echogram;
    float backgroundCutoff;
    QList<ImageFilter*> filters;
    RemoveBackground *backgroundFilter;
    MapDecibelToChar *decibelToCharFilter;
    MapVoltageToChar *voltageToCharFilter;
    MapAngleToChar *angleToCharFilter;
    ContrastGammaFilter *contrastGammaFilter;
    MedianFilter *medianFilter;
    ConvolveFilter *convolveFilter;
    LowPassFilter *lowpassFilter;
    int currentFrameNumber;

public:
    EWidget(QWidget *parent);
    bool getHasChanged();
    void setBackgroundCutoff(float value);
    void setColorMap(Color *newMap);
    void setColors(QColor c1, QColor c2, QColor c3);
    void setData(SonarData *newData);
    void setDisplayTransparency(bool value);
    void setFlip(bool doFlip);
    void setHasChanged(bool value);
    void setGammaContrast(float g, float c);
    void setLowerThreshold(float value);
    void setMaxRange(float value);
    void setMinRange(float value);
    void setRotate(bool rotation);
    void setScalers(float rs, float ts);
    void setThresholds(float lower, float upper);
    void setUpperThreshold(float value);
    void update();

public slots:
    void setCurrentFrameNumber(int value);
    void setDoConvolve(bool value);
    void setDoLowpass(bool value);
    void setDoMedian(bool value);
    void setFilterBackground(bool value);

protected:
    void addMark(QPoint p,int dir);
    void addAutoMarks();
    void clearSelection();
    void createFilters();
    void drawFileLocation(QPainter *painter);
    void exportEchoes();
    void fillFilterArray();
    QImage* getImage(QRect rect);
    void mapColors(QImage *image,unsigned char* data,Rectangle rect);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:
    void updateGeometry(int h, int v);  
    void goToFrameNumber(int frameNumber);

};

#endif // EWIDGET_H
