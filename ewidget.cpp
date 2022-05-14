#include "ewidget.h"

//---------------------------------------------------------------
//  Constructor
//---------------------------------------------------------------
EWidget::EWidget(QWidget *parent)
        :QWidget(parent)
{
    sonarData = nullptr;
    echogram = nullptr;
    rotate = true;
    flip = false;
    filterBackground = false;
    drawing = false;
    doLowpass = false;
    doMedian = false;
    doConvolve = false;
    hasChanged = false;
    dragging = false;
    backgroundColor = qRgb(255,255,255);
    timeScaler = 1.0;
    rangeScaler = 1.0;
    lowerThreshold = 0.0;//-40.0;
    upperThreshold = 95.0;//0.0;
    gammaValue = 1.0;
    contrastValue = 0.0;
    //setMouseTracking(true);
    backgroundCutoff = float(1.9);
    maxVoltage = float(0.1);
    createFilters();
    minMarkRange = 0.0;
    maxMarkRange = 1000.0;
    currentFrameNumber = 0;
}

//---------------------------------------------------------------
//  createFilters()
//  Initializes the image processing filters
//---------------------------------------------------------------
void EWidget::createFilters(){
    float *tempKernel = new float[9];

    //intialize a gausian kernel
    tempKernel[0] = tempKernel[2] = tempKernel[6] = tempKernel[8] = 1.0/16.0;
    tempKernel[1] = tempKernel[3] = tempKernel[5] = tempKernel[7] = 2.0/16.0;
    tempKernel[4] = 4.0/16.0;

    backgroundFilter = new RemoveBackground();
    decibelToCharFilter = new MapDecibelToChar();
    voltageToCharFilter = new MapVoltageToChar();
    angleToCharFilter = new MapAngleToChar();
    contrastGammaFilter = new ContrastGammaFilter();
    convolveFilter = new ConvolveFilter();
    medianFilter = new MedianFilter();
    lowpassFilter = new LowPassFilter();

    backgroundFilter->setCutoff(backgroundCutoff);
    backgroundFilter->setRotate(rotate);
    decibelToCharFilter->setLower(lowerThreshold);
    decibelToCharFilter->setUpper(upperThreshold);
    voltageToCharFilter->setLower(0.0);
    voltageToCharFilter->setUpper(maxVoltage);
    angleToCharFilter->setLimit(0.0);
    contrastGammaFilter->setContrast(contrastValue);
    contrastGammaFilter->setGamma(gammaValue);
    convolveFilter->setKernel(tempKernel);
}

//---------------------------------------------------------------
//  drawFileLocation()
//  Draws the line and triangle for to mark the location of the file
//---------------------------------------------------------------
void EWidget::drawFileLocation(QPainter *painter){
    QPen pen;
    if(sonarData){
        QPainterPath triangle;
        int x = rect().size().width()*currentFrameNumber/sonarData->getNumberOfPings();
        QColor color  = QColor(255,255,255,150);

        triangle.moveTo(x,5);
        triangle.lineTo(x-5,0);
        triangle.lineTo(x+5,0);
        triangle.lineTo(x,5);
        pen.setWidth(1);
        pen.setColor(color);      //White
        painter->setPen(pen);
        painter->drawLine(x,5,x,rect().size().height());
        painter->fillPath(triangle,QBrush(color,Qt::SolidPattern));
    }
}

//---------------------------------------------------------------
//  fillFilterArray()
//  Fills the list of image processing filters based on which are
//  selected.  The order is currently hard coded.
//---------------------------------------------------------------
void EWidget::fillFilterArray(){
    filters.clear();
    if(filterBackground){
        filters.append(backgroundFilter);
    }
    else{
        filters.append(decibelToCharFilter);
    }
    //filters.append(contrastGammaFilter);
    if(doMedian) filters.append(medianFilter);
    if(doLowpass) filters.append(lowpassFilter);
    if(doConvolve) filters.append(convolveFilter);

}

//---------------------------------------------------------------
//  getHasChanged()
//  Returns the hasChanged flag to determine whether to prompt the
//  user to save changes.
//---------------------------------------------------------------
bool EWidget::getHasChanged(){
    return hasChanged;
}

//---------------------------------------------------------------
//  getImage()
//  Retrieves the echogram image.  Currently this is the raw image
//  size not adjusted to the size of the widget.
//---------------------------------------------------------------
QImage* EWidget::getImage(QRect rect){
    Rectangle r;
    QImage* image;
    QImage* image2 = new QImage(this->rect().size(),QImage::Format_ARGB32);
    float *data, *data2;

    if(rotate){
        r.x = 0; r.y = 0; r.w=sonarData->getNumberOfPings()-1; r.h = sonarData->getNumberOfSamples()-1;
    }
    else{
        r.x = 0; r.y = 0; r.w=sonarData->getNumberOfSamples()-1; r.h = sonarData->getNumberOfPings()-1;
    }
    image = new QImage(r.w,r.h,QImage::Format_ARGB32);
    data = sonarData->getDecibels(r,minMarkRange,maxMarkRange);
    data2 = reinterpret_cast<float*>(mapDecibelToChar(data,r.w,r.h,lowerThreshold,upperThreshold));
    //  Colorize image
    mapColors(image,reinterpret_cast<unsigned char*>(data2),r);

    delete [] data;
    delete [] data2;

    return image;
}

//---------------------------------------------------------------
//  mapColors()
//  Sets the RGB color values of the passed QImage based on the
//  unsigned char values in the passed data.
//---------------------------------------------------------------
void EWidget::mapColors(QImage *image,unsigned char* data,Rectangle rect){
    int i,j,index;
    QRgb value;
    Color *map = colorMap;
    QRgb *line;

    for(i = 0; i < rect.h; i++){
        line = reinterpret_cast<QRgb*>(image->scanLine(i));
        for(j = 0; j < rect.w; j++){
            index = min(rect.w*rect.h-1,i*rect.w+j);
            index = data[index];
            value = qRgb(map[index].red,map[index].green,map[index].blue);
            line[j] = value;
        }
    }
}

void EWidget::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()==Qt::LeftButton & dragging){
        int x = event->x();
        int y = event->y();
        int frame = int(float(sonarData->getNumberOfPings())*(float(x)/float(rect().size().width())));
        emit goToFrameNumber(frame);
        setCurrentFrameNumber(frame);
    }
}

//---------------------------------------------------------------
//  mousePressEvent()
//  Responds to a mouse pressed event to initate dragging
//---------------------------------------------------------------
void EWidget::mousePressEvent(QMouseEvent *event){
    int x = event->x();
    int y = event->y();
    int x2 = rect().size().width()*currentFrameNumber/sonarData->getNumberOfPings();    //Location of file marker
    int frame = int(float(sonarData->getNumberOfPings())*(float(x)/float(rect().size().width())));
    //qDebug() << "Mouse pressed, go to frame:" << frame;
    emit goToFrameNumber(frame);
    setCurrentFrameNumber(frame);
    if((x>=x2-5)&&(x2<=x2+5)&&y<=5){
        dragging = true;
    }
}

void EWidget::mouseReleaseEvent(QMouseEvent *event){
    dragging = false;
}

//---------------------------------------------------------------
//  paintEvent()
//  Overloads paint event member function to draw the echogram
//---------------------------------------------------------------
void EWidget::paintEvent(QPaintEvent *event){    
    if(sonarData){
        QRect r = this->rect();//event->rect();
        QPainter *painter = new QPainter(this);
        //QImage* image = getImage(r);
        QRect *source;

        drawing = true;
        if(rotate){
            source = new QRect(0,0,min(sonarData->getNumberOfPings()-1,int(r.width()/1.0)),min(sonarData->getNumberOfSamples()-1,int(r.height()/1.0)));
        }
        else{
            source = new QRect(0,0,min(sonarData->getNumberOfSamples()-1,int(r.width()/1.0)),min(sonarData->getNumberOfPings()-1,int(r.height()/1.0)));
        }
        painter->setRenderHint(QPainter::SmoothPixmapTransform,true);
        painter->fillRect(r,backgroundColor);
        if(hasChanged){
            if(echogram){
                delete echogram;
            }

            echogram = getImage(r);
        }
        painter->drawImage(this->rect(),*echogram,echogram->rect());
        //painter->drawImage(QPoint(0,0),*echogram);
        drawFileLocation(painter);

        //delete image;
        delete source;
        delete painter;
        drawing = false;
    }
    else{
        QRect r = event->rect();
        QPainter *painter = new QPainter(this);
        painter->fillRect(r, QColor(10,10,10,255));
        delete painter;
    }
}

//---------------------------------------------------------------
//  setBackgroundCutoff()
//---------------------------------------------------------------
void EWidget::setBackgroundCutoff(float value){
    backgroundCutoff = value;
    backgroundFilter->setCutoff(backgroundCutoff);
}

//---------------------------------------------------------------
//  setColorMap()
//  Sets the colormap and repaints the echogram
//---------------------------------------------------------------
void EWidget::setColorMap(Color *newMap){
    colorMap = newMap;
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
//  setColors()
//  Sets the colors for the background for transparent colors and
//  the marks for both upstream and downstream.
//---------------------------------------------------------------
void EWidget::setColors(QColor c1, QColor c2, QColor c3){
    backgroundColor = c1;
    upstreamColor = c2;
    downstreamColor = c3;
    //repaint(rect());
}

//---------------------------------------------------------------
//  setCurrentFrameNumber()
//  Sets the frame currently showing in video and draws line marker
//  in echogram at that location.
//---------------------------------------------------------------
void EWidget::setCurrentFrameNumber(int value){
    if(abs(currentFrameNumber-value)>4){
        currentFrameNumber = value;
        repaint(rect());
    }
}

//---------------------------------------------------------------
//  setData()
//  If data already exists, delete it, then set the data pointer
//  to the new data and resize the echogram drawing canvas
//---------------------------------------------------------------
void EWidget::setData(SonarData *newData){
    sonarData = newData;
    if(sonarData){
        sonarData->setRotate(rotate);
        sonarData->setFlip(flip);
    }
    hasChanged = true;
}

//---------------------------------------------------------------
//  setDoConvolve()
//  Sets doConvolve flag.
//---------------------------------------------------------------
void EWidget::setDoConvolve(bool value){
    doConvolve = value;
    this->fillFilterArray();
}

//---------------------------------------------------------------
//  setDoLowpass()
//  sets doLowpass flag.
//---------------------------------------------------------------
void EWidget::setDoLowpass(bool value){
    doLowpass = value;
    this->fillFilterArray();
}

//---------------------------------------------------------------
//  setDoMedian()
//  sets doMedian flag.
//---------------------------------------------------------------
void EWidget::setDoMedian(bool value){
    doMedian = value;
    this->fillFilterArray();
}


//---------------------------------------------------------------
//  setFilterBackground()
//  Sets the remove backbround flag and redraws the echogram.
//---------------------------------------------------------------
void EWidget::setFilterBackground(bool value){
    filterBackground = value;
    this->fillFilterArray();
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
//  setFlip()
//  Sets which side is range=0.
//---------------------------------------------------------------
void EWidget::setFlip(bool doFlip){
    flip = doFlip;
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
//  setGammaContrast()
//  Sets the gamma and contrast of the image
//---------------------------------------------------------------
void EWidget::setGammaContrast(float g, float c){
    gammaValue = g;
    contrastValue = c;
    contrastGammaFilter->setContrast(contrastValue);
    contrastGammaFilter->setGamma(gammaValue);
    decibelToCharFilter->setContrastGamma(contrastValue,gammaValue);
    voltageToCharFilter->setContrastGamma(contrastValue,gammaValue);
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
//  setHasChanged()
//  Sets the hasChanged flag when a file is saved.
//---------------------------------------------------------------
void EWidget::setHasChanged(bool value){
    hasChanged = value;
}

//---------------------------------------------------------------
//  setLowerThreshold()
//  Sets the lower threshold and redraws widget.
//---------------------------------------------------------------
void EWidget::setLowerThreshold(float value){
    lowerThreshold = value;
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
// setMaxRange()
// Sets the maximum range at which mark clicks will be accepted.
//---------------------------------------------------------------
void EWidget::setMaxRange(float value){
    maxMarkRange = value;
    hasChanged = true;
    repaint(rect());
}

void EWidget::setMinRange(float value){
    minMarkRange = value;
}

//---------------------------------------------------------------
//  setRotate()
//  Sets the rotation of the echogram
//---------------------------------------------------------------
void EWidget::setRotate(bool rotation){
    rotate = rotation;
    backgroundFilter->setRotate(rotate);
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
//  setScalers()
//  Sets the range and time scalers to change the magnification
//  of the echogram.
//---------------------------------------------------------------
void EWidget::setScalers(float rs, float ts){
    rangeScaler = rs;
    timeScaler = ts;
    //qDebug() << "Range Scaler:" << rangeScaler << "  Time Scaler:" << timeScaler;
    //repaint(rect());
}

//---------------------------------------------------------------
//  setThresholds()
//  Sets the thresholds and repaints the echogram
//---------------------------------------------------------------
void EWidget::setThresholds(float lower, float upper){
    lowerThreshold = lower;
    upperThreshold = upper;
    decibelToCharFilter->setLower(lowerThreshold);
    decibelToCharFilter->setUpper(upperThreshold);
    hasChanged = true;
    repaint(rect());
}

//---------------------------------------------------------------
//  setUpperThreshold()
//  Sets the lower threshold and redraws widget.
//---------------------------------------------------------------
void EWidget::setUpperThreshold(float value){
    upperThreshold = value;
    hasChanged = true;
    repaint(rect());
}

void EWidget::update(){
    repaint(rect());
}

