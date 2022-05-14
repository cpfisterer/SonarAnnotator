#include "vwidget.h"

VWidget::VWidget(QWidget *parent):QWidget(parent)
{
    zoomSize=70;
    zoomImage = new QImage(zoomSize,zoomSize,QImage::Format_ARGB32);
    frame = nullptr;
    didsonData = nullptr;
    colorMap = nullptr;
    imageMap = nullptr;
    widthExpansion = 4;
    lowerThreshold = 0;//-40.0;
    upperThreshold = 95;//0.0;
    location = 0;
    backgroundCutoff = 10.0;
    flip = true;
    dragging = false;
    reverse = true;
    backgroundRemove = false;
    backgroundSubtract = false;
    averageFrames = false;
    useMedianFilter = false;
    gridColor = QColor(200,200,200,75);
    locationColor = QColor(255,255,255,255);
    setMouseTracking(true);
    usePerspective = true;
    annotations = new AnnotationList();
    selectedAnnotation = nullptr;
}

void VWidget::addAnnotation(QString category){
    if(selectionRect.width()>1 && selectionRect.height()>1){
        Annotation *a = new Annotation();
        strcpy(a->category,category.toLocal8Bit());
        a->frame = frame->getFrameNumber();
        a->x = float(selectionRect.x())/float(this->rect().width());
        a->y = float(selectionRect.y())/float(this->rect().height());
        a->width = float(selectionRect.width())/float(this->rect().width());
        a->height = float(selectionRect.height())/float(this->rect().height());
        annotations->add(a);
        emit annotationsChanged();
        dragging = false;
        dragStart.setX(0);
        dragStart.setY(0);
        selectionRect.setSize(QSize(0,0));
        update(rect());
    }
}

void VWidget::broadcastAnnotationChange(){
    emit annotationsChanged();
}

void VWidget::deleteAnnotation(int index){
    annotations->remove(index);
    emit annotationsChanged();
    update();
}

void VWidget::drawAnnotations(QPainter *painter){
    if(annotations->count()>0){
        for(int i = 0;i<annotations->count();i++){
            Annotation *a = annotations->get(i);
            if(a->frame==frame->getFrameNumber()){
                QRect r = QRect(int(float(rect().width())*a->x),
                                int(float(rect().height())*a->y),
                                int(float(rect().width())*a->width),
                                int(float(rect().height())*a->height));
                QColor border = QColor(255,255,255,200);
                painter->setPen(border);
                painter->drawRect(r);
                QFont font("Arial");
                painter->setPen(QColor(255,255,255,200));
                font.setPixelSize(9);
                painter->setFont(font);
                painter->drawText(int(float(rect().width())*a->x),
                                  int(float(rect().height())*a->y)-2,
                                  QString(a->category));
            }
        }
    }
    if(selectedAnnotation && annotations->count()>0){
        if(frame->getFrameNumber()==selectedAnnotation->frame){
            Annotation *a = selectedAnnotation;
            if(a->frame==frame->getFrameNumber()){
                QRect r = QRect(int(float(rect().width())*a->x),
                                int(float(rect().height())*a->y),
                                int(float(rect().width())*a->width),
                                int(float(rect().height())*a->height));
                QPalette sysPalette = qApp->palette();
                QPen pen = QPen(sysPalette.color(QPalette::Highlight));
                pen.setWidth(3.0);
                painter->setPen(pen);
                painter->drawRect(r);
            }
        }
    }
}

void VWidget::drawFrameNumber(QPainter *painter){
    QString s = tr("Frame: %1").arg(int(getFrameNumber()));

    painter->setPen(QColor(255,255,255,255));
    painter->drawText(5,rect().height()-4,s);
}

//---------------------------------------------------------------
//  drawGrid()
//  Draw the range and angle grid over the video.
//---------------------------------------------------------------
void VWidget::drawGrid(QPainter *painter){
    QRect r;
    int numberOfRanges=5,i;
    float range,samplesPerMeter = float(rect().height())/frame->getWindowLength(),startRange = frame->getWindowStart();
    float windowLength = frame->getWindowLength(),degToRad=3.1415927f/180;
    float angles[5]={-1.0f*0.3f*48.0f,-7.0f,0.0f,7.0f,0.3f*48.0f};
    float x,y;
    int x1,y1,x2,y2;

    //  Draw arcs
    if(didsonData->isBigLens()){    //divide angles by two to correct for big lens
        for(i = 0; i < 5; i++){
            angles[i] = angles[i]/2;
        }
    }
    painter->setPen(getLineColor());
    painter->setRenderHint(QPainter::Antialiasing,true);
    for(i = 0; i < numberOfRanges+1; i++){
        range = windowLength/float(numberOfRanges)*float(i)+startRange;
        r.setX(rect().width()/2-int(range*samplesPerMeter));
        r.setWidth(int(range*samplesPerMeter)*2);
        if(!flip){
            r.setY(int((range-startRange)*samplesPerMeter));
            r.setHeight(-1*r.width());
            if(didsonData->isBigLens()){
                painter->drawArc(r,int(-1*82.8*16),int(-28.8*16/2));
            }
            else{
                painter->drawArc(r,int(-1*75.6*16),int(-28.8*16));
            }
        }
        else{
            r.setY(rect().height()-int((range-startRange)*samplesPerMeter));
            r.setHeight(r.width());
            if(didsonData->isBigLens()){
                painter->drawArc(r,int(90+14.4/2)*16,int(-28.8*16/2));
            }
            else{
                painter->drawArc(r,int(90+14.4)*16,int(-28.8*16));
            }

        }
    }
    //  Draw lines
    for(i = 0; i < numberOfRanges; i++){
        float angle = angles[i];
        x = float(startRange)*float(sin(degToRad*angle));
        y = float(startRange*cos(degToRad*angle));
        x1 = int(samplesPerMeter*x+rect().width()/2);
        y1 = rect().height()-int((y-startRange)*samplesPerMeter);
        x = float((startRange+windowLength)*sin(degToRad*angle));
        y = float(startRange+windowLength*cos(degToRad*angle));
        x2 = int(samplesPerMeter*x+rect().width()/2);
        y2 = rect().height()-int((y-startRange)*samplesPerMeter);
        if(!flip){
            y1 = rect().height()-y1;
            y2 = rect().height()-y2;
            painter->drawLine(x1,y1,x2,y2);
        }
        painter->drawLine(x1,y1,x2,y2);
    }

}

//---------------------------------------------------------------
//  drawRangeLabels()
//  Draw the range labels along the rights side of the video
//---------------------------------------------------------------
void VWidget::drawRangeLabels(QPainter *painter){
    int numberOfRanges=5;
    float startRange = frame->getWindowStart();
    float windowLength = frame->getWindowLength();
    float samplesPerMeter=float(rect().height()/windowLength),degToRad=float(3.1415927/180.0);
    QFont font("Arial");

    painter->setPen(getTextColor());

    font.setPixelSize(13);
    painter->setFont(font);

    for(int i=0;i<numberOfRanges+1;i++){
        float range=windowLength/numberOfRanges*i+startRange;
        float angle = 14.5;
        float x,y;

        if(didsonData->isBigLens()){
            angle = angle/2;
        }
        x=samplesPerMeter*range*float(sin(angle*degToRad))+4;
        y=samplesPerMeter*(range*float(cos(angle*degToRad))-startRange);
        QString msg = tr("%1").arg(double(int(range*10.0f))/10.0);
        if(i==0) msg.append(" meters");
        x=x+rect().width()/2.0f;
        if(x+25>rect().width()) x=rect().width()-25;    //To try and prevent range from drawing beyond edge of video
        if(flip){
            y = rect().height()-y;
        }
        y=float(fmax(fmin(y, rect().height()-4),12));
        painter->drawText(int(x),int(y),msg);
    }
}

void VWidget::drawSelection(QPainter *painter){
    if(dragging && selectionRect.width()>1 && selectionRect.height()>1){
        QColor border = QColor(255,255,255,200);
        QColor fill = QColor(175,175,175,150);
        painter->fillRect(selectionRect,fill);
        painter->setPen(border);
        painter->drawRect(selectionRect);
    }
}

//---------------------------------------------------------------
//  expandImage()
//  Expands the width of the data by doing a linear interpolation
//  between beams.  Note that the number of interpolated beams is
//  the widthExpansion-1.
//---------------------------------------------------------------
unsigned char* VWidget::expandImage(unsigned char* data, int w, int h){
    unsigned char* newData = new unsigned char[w*h*widthExpansion];
    int i,j,k;

    for(i = 0; i < w; i++){
        for(j = 0; j < h; j++){
            newData[j*w*widthExpansion+i*widthExpansion] = data[j*w+i];
            for(k = 1; k < widthExpansion; k++){
                //newData[j*(w*widthExpansion)+i*widthExpansion+k] = data[j*w+i];
                if(i < (w-1)){
                    newData[j*(w*widthExpansion)+i*widthExpansion+k] = uchar(float((widthExpansion-k)*data[j*w+i]+k*data[j*w+i+1])/float(widthExpansion));
                    //newData[j*(w*widthExpansion)+i*widthExpansion+k] = (float)(k*(data[j*w+i]-data[j*w+i+1]))/(float)widthExpansion+data[j*w+i];
                }
                else{
                    newData[j*(w*widthExpansion)+i*widthExpansion+k] = data[j*w+i];      //Case for the last beam
                }
            }
        }
    }

    return newData;
}

//---------------------------------------------------------------
//  exportMap()
//  Exports a text file containing the sample number corresponding
//  to the pixel location for each pixel in the display.  Used for debugging.
//---------------------------------------------------------------
void VWidget::exportMap(){
    int i,j;
    int width = rect().width(),height = rect().height();
    char filename[]="ImageMap.csv";
    FILE *file;

    qDebug() << "Exporting Image Map";
    if((file=fopen(filename,"w"))==nullptr){
        qDebug() << "Can't open" << filename;
        return;
    }
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            fprintf(file,"%i,",imageMap[i*width+j]);
        }
        fprintf(file,"\n"); //Print newline
    }
    fclose(file);
}

//---------------------------------------------------------------
//  fileLocation()
//  Returns the current file location in percent of file.
//---------------------------------------------------------------
float VWidget::fileLocation(){
    if(frame){
        float location = float(frame->getFrameNumber())/float(didsonData->getNumberOfPings())*100.0f;
        //qDebug() << "File location:" << location << "Frame number:" << frame->getFrameNumber() << "Number of frames:" << didsonData->getNumberOfPings();
        return fmin(100.0f,location);
    }
    else{
        return 0;
    }
}

//---------------------------------------------------------------
//  flipVertically()
//  Flips the image map vertically so that 0 meters is on the top
//  or bottom.
//---------------------------------------------------------------
void VWidget::flipVertically(){
    int w = rect().width(),h=rect().height();
    int *newImageMap = new int[w*h];

    for(int j = 0; j < h; j++){
        for(int i = 0; i < w; i++){
            newImageMap[i+j*w]=imageMap[i+(h-1-j)*w];
        }
    }
    delete [] imageMap;
    imageMap = newImageMap;
}

//---------------------------------------------------------------
//  flipHorizontally()
//  Flips the image map to change direction of travel.
//---------------------------------------------------------------
void VWidget::flipHorizontally(){
    int w = rect().width(),h=rect().height();
    int *newImageMap = new int[w*h];

    for(int j = 0; j < h; j++){
        for(int i = 0; i < w; i++){
            newImageMap[i+j*w]=imageMap[(w-1-i)+j*w];
        }
    }
    delete [] imageMap;
    imageMap = newImageMap;
}

AnnotationList* VWidget::getAnnotationList(){
    return annotations;
}

//---------------------------------------------------------------
//  getAverageImage()
//  Retrieves the frame that is the average of the previous and
//  next frame.  A primitive lowpass filter to reduce noise.
//---------------------------------------------------------------
float *VWidget::getAverageImage(){
    int current = frame->getFrameNumber();
    float *data, *data2 = nullptr, *data3, *data4;

    data = frame->getFrame();
    data2 = duplicateImage(data,int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));//Current frame
    frame->readFrame();
    data = frame->getFrame();
    data3 = duplicateImage(data,int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));//Next frame
    frame->goToFrame(int(fmax(0,current - 1)));
    data = frame->getFrame();
    data4 = duplicateImage(data,int(frame->getNumberOfBeams()),int(frame->getNumberOfSamples()));//Previous frame
    frame->readFrame();
    data = averageImage(data2, data3, data4, int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));
    delete [] data2;
    delete [] data3;
    delete [] data4;

    return data;
}

//---------------------------------------------------------------
//  getMedianImage()
//  Retrieves frame that is median of the previous, current, and
//  next frame.
//---------------------------------------------------------------
float *VWidget::getMedianImage(){
    int current = frame->getFrameNumber();
    float *data, *data2 = nullptr, *data3, *data4;

    data = frame->getFrame();
    data2 = duplicateImage(data,int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));//Current frame
    frame->readFrame();
    data = frame->getFrame();
    data3 = duplicateImage(data,int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));//Next frame
    frame->goToFrame(int(fmax(0,current - 1)));
    data = frame->getFrame();
    data4 = duplicateImage(data,int(frame->getNumberOfBeams()),int(frame->getNumberOfSamples()));//Previous frame
    frame->readFrame();
    data = medianImage(data2, data3, data4, int(frame->getNumberOfBeams()),int(frame->getNumberOfSamples()));
    delete [] data2;
    delete [] data3;
    delete [] data4;

    return data;
}



//---------------------------------------------------------------
//  getImage()
//  Returns processed frame image adjusted for perspective.
//---------------------------------------------------------------
QImage *VWidget::getImage(){
    float *data, *data2 = nullptr;
    unsigned char *imageData, *imageData2;
    QImage *image = new QImage(rect().width(),rect().height(),QImage::Format_ARGB32);

    if(averageFrames) data = getAverageImage();
    else if(useMedianFilter) data = getMedianImage();
    else data = data = duplicateImage(frame->getFrame(), int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));
    if(backgroundRemove){
        data2 = removeBackground(data, didsonData->getVideoBackground(), this->getFrameNumber(), didsonData->getNumberOfPings(),
                                 int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()), backgroundCutoff);
        //data2 = didsonData->getBackgroundFrame(this->getFrameNumber());  //Use this instead of the previous line to display background image
    }
    else if(backgroundSubtract){
        data2 = subtractBackground(data, didsonData->getVideoBackground(), this->getFrameNumber(), didsonData->getNumberOfPings(),
                                 int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));
    }
    else{
        data2 = duplicateImage(data, int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));
    }
    frame->correctTVG(data2);
    imageData = mapDecibelToChar(data2, int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()), lowerThreshold, upperThreshold);
    //imageData2 = adjustContrastGamma(imageData,frame->getNumberOfBeams(),512,contrastValue,gammaValue);
    imageData2 = expandImage(imageData, int(frame->getNumberOfBeams()), int(frame->getNumberOfSamples()));
    mapColorsPerspective(image,imageData2);

    delete [] data;
    delete [] data2;
    delete [] imageData;
    delete [] imageData2;

    return image;
}

//---------------------------------------------------------------
//  getCurrentTime()
//  Returns a string containing the time of the currently
//  displayed frame.
//---------------------------------------------------------------
QString VWidget::getCurrentTime(){
    if(frame){
        char *time = frame->getDateTimeString();
        QString dateTime = QString(time);
        delete [] time;

        return dateTime;
    }
    else{
        return QString(tr("0"));
    }
}

//---------------------------------------------------------------
//  getFrameNumber()
//  Returns the frame number of the currently displayed frame.
//---------------------------------------------------------------
int VWidget::getFrameNumber(){
    if(frame){
        return frame->getFrameNumber();
    }
    else{
        return 0;
    }
}

//---------------------------------------------------------------
//  getLineColor()
//  Determines the colors of lines depending upon the brigtness of the
//  background color;
//---------------------------------------------------------------

QColor VWidget::getLineColor(){
    int lightness = QColor(colorMap[0].red,colorMap[0].green,colorMap[0].blue).lightness();

    if(lightness < 128){
        return QColor(200,200,200,75);
    }
    return QColor(56,56,56,75);
}


int VWidget::getNumberOfFrames(){
    if(didsonData){
        return didsonData->getNumberOfPings();
    }
    else{
        return 0;
    }
}

//---------------------------------------------------------------
//  getTextColor()
//  Returns color based on the background color of the image map
//  to try and maintain contrast.
//---------------------------------------------------------------
QColor VWidget::getTextColor(){
    int lightness = QColor(colorMap[0].red,colorMap[0].green,colorMap[0].blue).lightness();

    if(lightness < 128){
        return QColor(255,255,255,255);
    }
    return QColor(0,0,0,255);
}

//---------------------------------------------------------------
//  goToFrame()
//  Jumps to the frame passed.
//---------------------------------------------------------------
void VWidget::goToFrame(int value){
    if(frame && value<(didsonData->getNumberOfPings()-2)){
        if(int(fmin(int(didsonData->getNumberOfPings()*(float(value)/100.0f)),didsonData->getNumberOfPings()-2))){
            frame->goToFrame(value);
        }
        update();
    }
}

//---------------------------------------------------------------
//  goToFileLocation()
//  Jumps to the location in the file passed as a percent of the
//  total number of frames.
//---------------------------------------------------------------
void VWidget::goToFileLocation(int value){
    if(frame){
        frame->goToFrame(int(fmin(int(didsonData->getNumberOfPings()*(float(value)/100.0f)),didsonData->getNumberOfPings()-2)));
        update();
    }
}

//---------------------------------------------------------------
//  initializeMap()
//  Setup the lookup table maping display space to data space.
//---------------------------------------------------------------

void VWidget::initializeMap(){
    if(frame){
        if(imageMap){delete [] imageMap;}
        imageMap = new int[rect().width()*rect().height()];
        if(usePerspective){
            initializePerspectiveMap();
        }
        else{
            initializeSquareMap();
        }
        if(flip && reverse){
            flipVertically();
        }
        else if(flip && !reverse){
            flipHorizontally();
            flipVertically();
        }
        else if(!flip && reverse){
            flipHorizontally();
        }
        //exportMap();
    }
}

//---------------------------------------------------------------
//  initializePerspectiveMap()
//  Maps data space to imagespace correcting for beam angle.
//---------------------------------------------------------------
void VWidget::initializePerspectiveMap(){
    double numberOfBeams = double(frame->getNumberOfBeams());
    int numSamples = int(frame->getNumberOfSamples());
    double phi,radToDeg=180.0/3.1415927;
    double startRange = double(frame->getWindowStart());
    double R,angle,sample,beamNum,x,y;
    int i,j,index;
    int width = rect().width(), height = rect().height();           //width and height of the video viewer
    double pixelsPerMeter = double(height)/double(frame->getWindowLength());

    phi = double(didsonData->getHorizontalBeamWidth());
    phi = phi/double(numberOfBeams);
    phi = phi/double(widthExpansion);
    numberOfBeams = numberOfBeams*widthExpansion;

    for(j = 0; j < height; j++){
        for(i = 0; i < width; i++){
            x = (double(i)-double(width)/2.0)/pixelsPerMeter;
            y = double(j)/pixelsPerMeter+startRange;
            R = pow((pow(x,2.0)+pow(y,2.0)),0.5);
            angle=atan(x/y);
            angle=radToDeg*angle;
            sample=double(round((R-startRange)*(double(numSamples)/double(frame->getWindowLength()))));
            beamNum=double(round(angle/phi+numberOfBeams/2.0));
            index = i+j*width;
            if((sample<0)||(sample>(numSamples-1))||(beamNum>numberOfBeams)||(beamNum<0)){
                imageMap[index]=0;
            }
            else{
                imageMap[index]=int(fmin(round((sample*numberOfBeams+beamNum)),numSamples*numberOfBeams-1.0));
            }
        }
    }
}

//---------------------------------------------------------------
//  initializeSquareMap()
//  Maps data space to display space without correcting for beam angle.
//---------------------------------------------------------------
void VWidget::initializeSquareMap(){
    float numberOfBeams = float(frame->getNumberOfBeams()*widthExpansion);
    int numSamples = int(frame->getNumberOfSamples());
    int width = rect().width(), height = rect().height();
    int i,j,beam,sample;

    for(j = 0; j < height; j++){
        for(i = 0; i < width; i++){
            beam = int(numberOfBeams*(float(i)/float(width)));
            sample = int(float(numSamples)*(float(j)/float(height)));
            if((sample<0)||(sample>(numSamples-1))||(beam>numberOfBeams)||(beam<0)){
                imageMap[i+j*width]=0;
            }
            else{
                imageMap[i+j*width] = int(fmin(numSamples*int(numberOfBeams)-1.0f,sample*int(numberOfBeams)+beam));
            }
        }
    }
}

void VWidget::keyPressEvent(QKeyEvent *event){
    emit keyPressedInVideo(event);
}

//---------------------------------------------------------------
//  mapColorsPerspective()
//  Sets the RGB color values of the passed QImage based on the
//  unsigned char values in the passed data.  Also uses the image
//  map to correct for perspective.
//---------------------------------------------------------------
void VWidget::mapColorsPerspective(QImage *image, unsigned char* data){
    int i,j,w,h,index;
    QRgb value;
    Color *map = colorMap;
    QRgb* line;

    w = rect().width();
    h = rect().height();
    for(i = 0; i < h; i++){
        line = reinterpret_cast<QRgb*>(image->scanLine(i));
        for(j = 0; j < w; j++){
            index = data[imageMap[i*w+j]];
            value = qRgb(map[index].red,map[index].green,map[index].blue);
            //Use below if using a fixed color for area of image not in beam
            if(imageMap[i*w+j]==0){ //Set as the background color
                value = qRgb(0,0,0);
            }
            else{
                value = qRgb(map[index].red,map[index].green,map[index].blue);
            }
            line[j] = value;
        }
    }
}

void VWidget::mouseMoveEvent(QMouseEvent *event){
    if((event->buttons()==Qt::LeftButton) & dragging){
        int x = event->x();
        int y = event->y();
        selectionRect.setX(int(fmin(x,dragStart.x())));
        selectionRect.setY(int(fmin(y,dragStart.y())));
        selectionRect.setWidth(int(fabs(x-dragStart.x())));
        selectionRect.setHeight(int(fabs(y-dragStart.y())));
        this->update();
    }
}

//---------------------------------------------------------------
//  mousePressEvent()
//  Responds to mouse pressed events to measure in video using
//  multiple line segments.  This stills needs to be implemented.
//---------------------------------------------------------------
void VWidget::mousePressEvent(QMouseEvent *event){
    if(dragging){
        dragging = false;
        dragStart.setX(0);
        dragStart.setY(0);
    }
    else{
        dragging = true;
        dragStart.setX(event->x());
        dragStart.setY(event->y());
    }
    this->update();
}

//---------------------------------------------------------------
//  mouseReleaseEvent()
//  Responds to mouse released events to determine whether to
//  start a new line or a new segment.  If right button, ends the
//  current line so that the next left mouse press starts a new
//  line.  Still needs to be implemented.
//---------------------------------------------------------------
void VWidget::mouseReleaseEvent(QMouseEvent *event){
    if(event){}
}

//---------------------------------------------------------------
//  nextFrame()
//  Reads the next frame and refreshes the display.  If the next
//  frame exceeds the total number of frames, goes back to the
//  start of the file.
//---------------------------------------------------------------
void VWidget::nextFrame(){
    if(frame->getFrameNumber() < (didsonData->getNumberOfPings()-2)){
        frame->readFrame();
    }
    else{
        frame->goToFrame(0);
    }
    update();
}

//---------------------------------------------------------------
//  paintEvent()
//  Overrides QWidget paintEvent.  Draws image to fill the
//  rectangular area.
//---------------------------------------------------------------
void VWidget::paintEvent(__attribute__((unused)) QPaintEvent *event){

    if(frame){
        QImage *image;
        QPainter *painter = new QPainter(this);

        image = getImage();
        painter->drawImage(rect(),*image);
        if(usePerspective){
            drawGrid(painter);
            drawRangeLabels(painter);
        }
        drawFrameNumber(painter);
        drawSelection(painter);
        drawAnnotations(painter);

        delete image;
        delete painter;
    }
    else{
        QRect r = event->rect();
        QPainter *painter = new QPainter(this);
        painter->fillRect(r, QColor(10,10,10,255));
        delete painter;
    }
}

//---------------------------------------------------------------
//  previousFrame()
//  Reads the previous frame and refreshes the display.  If the next
//  frame exceeds the total number of frames, goes back to the
//  start of the file.
//---------------------------------------------------------------
void VWidget::previousFrame(){
    if(frame->getFrameNumber() > 2){
        frame->goToFrame(frame->getFrameNumber()-2);
        frame->readFrame();
    }
    else{
        frame->goToFrame(1);
    }
    update();
}

//---------------------------------------------------------------
//  resetMark()
//  Clears the current path and resets the measuring flag.
//---------------------------------------------------------------
void VWidget::resetMark(){
    this->update();
}

//---------------------------------------------------------------
//  resizeEvent()
//  Overrides virtual function to call initializeMap() when the
//  video window is resized.
//---------------------------------------------------------------
void VWidget::resizeEvent (__attribute__((unused)) QResizeEvent * event ){
    initializeMap();
}

//---------------------------------------------------------------
//  setAverageFrames()
//  Sets the averageFrames flag.
//---------------------------------------------------------------
void VWidget::setAverageFrames(bool value){
    averageFrames = value;
    if(averageFrames){
        useMedianFilter = false;
    }
    update();
}

//---------------------------------------------------------------
//  setBackgroundCutoff()
//  Sets the cutoff value used in background removal.
//---------------------------------------------------------------
void VWidget::setBackgroundCutoff(float value){
    backgroundCutoff = value;
    update();
}

//---------------------------------------------------------------
//  setColorMap()
//  Sets the colormap and updates the display.
//---------------------------------------------------------------
void VWidget::setColorMap(Color *map){
    colorMap = map;
    update(rect());
}

//---------------------------------------------------------------
//  setData()
//  Called when a new file is opened and sets the data to be
//  displayed.
//---------------------------------------------------------------
void VWidget::setData(DIDSONData *data){
    didsonData = data;
    if(didsonData){
        frame = data->getFrame();
        reverse = data->getReverse();
        initializeMap();
        frame->goToFrame(0);
        annotations->clear();
    }
    else{
        frame = nullptr;
    }
}

//---------------------------------------------------------------
//  setFlip()
//  Sets the flip flag that determines whether the image is flipped
//  left to right.
//---------------------------------------------------------------
void VWidget::setFlip(bool value){
    flip = value;
    initializeMap();
    update();
}

//---------------------------------------------------------------
//  setLocation()
//  Sets the current range of the cursor in the echogram.  This
//  is displayed as a circle along the center of the beam.
//---------------------------------------------------------------
void VWidget::setLocation(float value){
    location = value;
}

//---------------------------------------------------------------
//  setLocationColor()
//  Sets the color of the range indicator.
//---------------------------------------------------------------
void VWidget::setLocationColor(QColor color){
    locationColor = color;
}

//---------------------------------------------------------------
//  setLowerThreshold()
//  Sets the lower threshold for image processing.
//---------------------------------------------------------------
void VWidget::setLowerThreshold(double value){
    lowerThreshold = float(value);
    update();
}

//---------------------------------------------------------------
//  setRemoveBackground()
//  Sets the remove background flag.
//---------------------------------------------------------------
void VWidget::setRemoveBackground(bool value){
    backgroundRemove = value;
    update();
}

//---------------------------------------------------------------
//  setReverse()
//  Sets reverse flag that determines whether image is flipped
//  top to bottom.
//---------------------------------------------------------------
void VWidget::setReverse(bool value){
    reverse = value;
    initializeMap();
    update();
}

void VWidget::setSelectedAnnotation(Annotation *a){
    selectedAnnotation = a;
    update();
}

//---------------------------------------------------------------
//  setSubtractBackground()
//  Sets the subtract background flag.
//---------------------------------------------------------------
void VWidget::setSubtractBackground(bool value){
    backgroundSubtract = value;
    update();
}

//---------------------------------------------------------------
//  setUpperThreshold()
//  Sets the upper threshold for image processing.
//---------------------------------------------------------------
void VWidget::setUpperThreshold(double value){
    upperThreshold = float(value);
    update();
}

void VWidget::setUseMedianFilter(bool value){
    useMedianFilter = value;
    if(useMedianFilter) averageFrames = false;
    update();
}

//---------------------------------------------------------------
//  toggleAverageFrames()
//  Toggles whether or not to average the frames preceeding and
//  following with the current frame.
//---------------------------------------------------------------
void VWidget::toggleAverageFrames(){
    if(averageFrames){
        averageFrames = false;
    }
    else{
        averageFrames = true;
        useMedianFilter = false;
    }
    update();
}

