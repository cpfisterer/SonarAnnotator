#ifndef VWIDGET_H
#define VWIDGET_H

#include <QApplication>
#include <QPalette>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QtGlobal>
#include <QImage>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QColor>
#include <QMouseEvent>
#include <QPixmap>
#include "imageprocessing.h"
#include "didsonframe.h"
#include "didsondata.h"
#include "arisdata.h"
#include "arisframe.h"
#include "annotationlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class VWidget : public QWidget
{
    Q_OBJECT
    DIDSONFrame *frame = nullptr;
    DIDSONData *didsonData = nullptr;
    Color *colorMap;
    int *imageMap;
    bool flip,reverse,dragging,backgroundRemove,backgroundSubtract;
    int widthExpansion;
    float lowerThreshold,upperThreshold;
    float location;
    float backgroundCutoff;
    QColor gridColor,locationColor;
    QPoint mouseLocation;
    QPoint dragStart;
    QRect selectionRect;
    QImage *zoomImage;
    int zoomSize;
    bool usePerspective,useMedianFilter;
    AnnotationList *annotations;
    Annotation *selectedAnnotation;

public:
    VWidget(QWidget *parent);
    void addAnnotation(QString category);
    bool averageFrames;
    void broadcastAnnotationChange();
    void deleteAnnotation(int index);
    float fileLocation();
    AnnotationList* getAnnotationList();
    QString getCurrentTime();
    int getFrameNumber();
    QImage *getImage();
    void resetMark();
    int getNumberOfFrames();
    void setColorMap(Color *map);
    void setData(DIDSONData *data);
    void setLocation(float value);
    void setLocationColor(QColor color);
    void setSelectedAnnotation(Annotation *a);

protected:
    void drawAnnotations(QPainter *painter);
    void drawFrameNumber(QPainter *painter);
    void drawGrid(QPainter *painter);
    void drawRangeLabels(QPainter *painter);
    void drawSelection(QPainter *painter);
    void exportMap();
    unsigned char* expandImage(unsigned char* data, int w, int h);
    void flipHorizontally();
    void flipVertically();
    float *getAverageImage();    
    QColor getTextColor();
    QColor getLineColor();
    float *getMedianImage();
    void initializeMap();
    void initializePerspectiveMap();
    void initializeSquareMap();
    void keyPressEvent(QKeyEvent *event);
    void mapColorsPerspective(QImage *image, unsigned char* data);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *event );

public slots:
    void goToFileLocation(int value);
    void goToFrame(int value);
    void nextFrame();
    void previousFrame();
    void setAverageFrames(bool value);
    void setBackgroundCutoff(float value);
    void setFlip(bool value);
    void setLowerThreshold(double value);
    void setRemoveBackground(bool value);
    void setReverse(bool value);
    void setSubtractBackground(bool value);
    void setUpperThreshold(double value);
    void setUseMedianFilter(bool value);
    void toggleAverageFrames();

signals:
    void locationChanged(QImage *image);
    void annotationsChanged();
    void keyPressedInVideo(QKeyEvent *event);
};

#endif // VWIDGET_H
