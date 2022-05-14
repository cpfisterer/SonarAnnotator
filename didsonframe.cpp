#include "didsonframe.h"

DIDSONFrame::DIDSONFrame()
{
}

DIDSONFrame::DIDSONFrame(FILE *newFile,int numBeams,int res, int ver){
    file = newFile;
    numberOfBeams = numBeams;
    numberOfSamples = 512;
    frame = new float[numberOfSamples*numberOfBeams];
    resolution = res;
    alpha = 0.0;
    tvg = 0.0;
    decibelCorrection = -100;
    avgBeams = 4;
    tvgValues = new float[numberOfSamples+2];
    calculateTVGValues();
    version = ver;
    if(version ==3){
        fileHeaderLength = 512;
        frameHeaderLength = 256;
    }
    else if(version ==4){
        fileHeaderLength = 1024;
        frameHeaderLength = 1024;
    }
    else{
        fileHeaderLength = 512;
        frameHeaderLength = 256;
    }
}

DIDSONFrame::~DIDSONFrame(){
    if(tvgValues){
        delete [] tvgValues;
    }
}

void DIDSONFrame::readFrameHeader(){
    int tempInt,startIndex,lengthIndex;
    float tempFloat,windowLengths;
    long long tempLong,currentOffset;
    double tempDouble;
    QDateTime tempTime;

    currentOffset=ftello(file);
    currentFrameNumber=SonarData::readInt(file);			//Frame number
    tempLong=SonarData::readLong(file);                     //Date/time value
    frameTime.setTime_t(uint(tempLong));
    tempInt = SonarData::readInt(file);                     //Version
    tempInt=SonarData::readInt(file);                       //Status
    year=SonarData::readInt(file);                          //Year
    month=SonarData::readInt(file);                         //Month
    day=SonarData::readInt(file);                           //Day
    hour=SonarData::readInt(file);                          //Hour
    minute=SonarData::readInt(file);                    	//Minute
    second=SonarData::readInt(file);                    	//Second
    tempInt=SonarData::readInt(file);                   	//Hundreths of second
    tempInt=SonarData::readInt(file);                   	//Transmitt mode
    startIndex=SonarData::readInt(file);					//Start index
    lengthIndex=SonarData::readInt(file);					//Length index
    threshold=SonarData::readInt(file);                     //Threshold
    intensity=SonarData::readInt(file);                     //Intensity
    gain=SonarData::readInt(file);                          //Gain
    temperature=SonarData::readInt(file);                   //Deg C #1
    tempInt=SonarData::readInt(file);                   	//Deg C #2
    humidity=SonarData::readInt(file);                   	//Humidity
    focus=float(SonarData::readInt(file));                  //Focus
    power=SonarData::readInt(file);                         //Battery
    fseeko(file,16,SEEK_CUR);                            	//Status 1
    fseeko(file,16,SEEK_CUR);                            	//Status 2
    tempFloat=SonarData::readFloat(file);					//Velocity
    tempFloat=SonarData::readFloat(file);					//Depth
    tempFloat=SonarData::readFloat(file);					//Altitude
    tempFloat=SonarData::readFloat(file);					//Pitch
    tempFloat=SonarData::readFloat(file);					//Pitch rate
    tempFloat=SonarData::readFloat(file);					//Roll
    tempFloat=SonarData::readFloat(file);					//Roll rate
    tempFloat=SonarData::readFloat(file);					//Heading
    tempFloat=SonarData::readFloat(file);					//Heading rate
    pan=SonarData::readFloat(file);                         //Pan
    tilt=SonarData::readFloat(file);                        //Tilt
    roll=SonarData::readFloat(file);                        //Sonar roll
    tempDouble=SonarData::readDouble(file);                 //Latitude
    tempDouble=SonarData::readDouble(file);             	//Longitude
    tempFloat=SonarData::readFloat(file);					//Sonar position
    configFlag=SonarData::readInt(file);					//Config flag
    configFlag = configFlag&0x0000000f;                     //Only use the last bit
    fseeko(file,60,SEEK_CUR);                                //Character buffer
    if(version == 4){
        fseeko(file,768,SEEK_CUR);
    }
    switch(configFlag){
    case 0: //DIDSON-S extended windows
        windowLengths = 1.25f;
        windowStart=float(startIndex)*(0.419f+0.419f*float(resolution));
        break;
    case 1: //DIDSON-S classic windows
        windowLengths = 1.125f;
        windowStart=float(startIndex)*(0.375f+0.375f*float(resolution));
        break;
    case 2: //DIDSON-LR extended windows
        windowLengths = 2.5f;
        windowStart=float(startIndex)*(0.419f+0.419f*float(resolution));
        break;
    case 3: //DIDSON-LR classic windows
        windowLengths = 2.25f;
        windowStart=float(startIndex)*(0.375f+0.375f*float(resolution));
        break;
    default:
        windowLengths = 1.125f;
        windowStart=float(startIndex)*(0.419f+0.419f*float(resolution));
        break;
    }
    windowLength = windowLengths*float(pow(2,lengthIndex+2*resolution));
    frameLength = frameHeaderLength+512*numberOfBeams;
    calculateTVGValues();
    //qDebug() << "Time=" << hour<<":"<<minute<<":"<< second <<" Long Value="<<tempLong<< " Gain="<<gain<<" Threshold="<<threshold;
}

void DIDSONFrame::readFrame(){
    this->readFrameNoTVG();
}

void DIDSONFrame::readFrameNoTVG(){
    unsigned char data[numberOfSamples*numberOfBeams];

    readFrameHeader();
    SonarData::readBuffer(file,reinterpret_cast<char*>(data),int(sizeof(unsigned char)*ulong(numberOfSamples*numberOfBeams)));
    correctAmplitude(data);
}

void DIDSONFrame::goToFrame(int frameNumber){
    fseeko(file,off_t(ulong(fileHeaderLength)+ulong((frameNumber*frameLength))),SEEK_SET);
    readFrame();
}

void DIDSONFrame::correctTVG(unsigned char *data){
    int i,j;
    float muPerDb=2.667f;		//Machine units per decibel

    for(i=0;i<numberOfBeams;i++){
        for(j=0;j<numberOfSamples;j++){
            //frame[j*numberOfBeams+i]=data[j*numberOfBeams+i]/muPerDb+tvgValues[j]+decibelCorrection;
            frame[j*numberOfBeams+i]=data[j*numberOfBeams+i]/muPerDb+tvgValues[j];
        }
    }
}

void DIDSONFrame::correctTVG(float *data){
    int i,j;

    for(i = 0; i < numberOfBeams; i++){
        for(j = 0; j < numberOfSamples; j++){
            data[j*numberOfBeams+i]=data[j*numberOfBeams+i]+tvgValues[j];
        }
    }
}

void DIDSONFrame::correctAmplitude(unsigned char *data){
    int i,j;
    float muPerDb=2.667f;		//Machine units per decibel

    for(i=0;i<numberOfBeams;i++){
        for(j=0;j<numberOfSamples;j++){
            //frame[j*numberOfBeams+i]=data[j*numberOfBeams+i]/muPerDb+decibelCorrection;
            frame[j*numberOfBeams+i]=data[j*numberOfBeams+i]/muPerDb;
        }
    }
}

float * DIDSONFrame::centerBeam(){
    float *center,avg;
    int temp1,temp2;
    float temp3;

    center = new float[numberOfSamples];                                // **** WARNING, may not be freeing data!
    temp1=numberOfBeams/2;
    for(int i=0;i<numberOfSamples;i++)
    {
        avg=0;
        for(int j=0;j<avgBeams;j++){
            temp2=i*numberOfBeams;
            temp3=frame[temp2+temp1+(j-int(floor(avgBeams/2)))];
            avg=avg+temp3;
        }
        avg=avg/avgBeams;
        center[i]=avg;
    }
    return center;
}

float * DIDSONFrame::maximumBeam(){
    float *center,max;
    int temp1,temp2;
    float temp3;

    center = new float[numberOfSamples];
    temp1 = numberOfBeams/2;
    for(int i = 0; i < numberOfSamples; i++){
        max = -500;
        for(int j = 0; j < avgBeams; j++){
            temp2 = i*numberOfBeams;
            temp3 = frame[temp2+temp1+(j-int(floor(avgBeams/2)))];
            if(temp3 > max) max = temp3;
        }
        center[i]=max;
    }
    return center;
}

float* DIDSONFrame::angles(){
    float max =-500,value=0,phi;
    int beamNumber=0;
    float* angles = new float[numberOfSamples];

    if(numberOfBeams<96) phi = 0.6f;       // phi is the degrees per beam
    else phi = 0.3f;

    for(int i=0;i<numberOfSamples;i++){
        max=-500;
        beamNumber=0;
        for(int j = 0; j < numberOfBeams; j++){
            value=frame[i*numberOfBeams+j];
            if(value>max){
                max=value;
                beamNumber=j;
            }
        }
        value = phi*(beamNumber-numberOfBeams/2);
        angles[i] = value;
    }
    return angles;
}


float *DIDSONFrame::getFrame(){
    return frame;
}

void DIDSONFrame::calculateTVGValues(){
    float range,samplesPerMeter = windowLength/numberOfSamples,correction = 0;
    int i;

    for(i=0;i<numberOfSamples+2;i++){
        range = windowStart+(i+1)*samplesPerMeter;
        correction = tvg*float(log10(range))+2.0f*alpha*range;
        tvgValues[i] = correction;
    }
}

float DIDSONFrame::getWindowStart(){
    return windowStart;
}

float DIDSONFrame::getWindowLength(){
    return windowLength;
}

float DIDSONFrame::getMaximumRange(){
    return windowStart+windowLength;
}

float DIDSONFrame::getNumberOfBeams(){
    return numberOfBeams;
}

float DIDSONFrame::getNumberOfSamples(){
    return float(numberOfSamples);
}

int DIDSONFrame::getFrameNumber(){
    return currentFrameNumber-firstFrame;
}

QString DIDSONFrame::getSystemType(){
    if(configFlag<=1){
        return QString("DIDSON - Standard");
    }
    return QString("DIDSON - Long Range");
}

//-----------------------------------------------------------
//  getDateTimeString()
//  Returns a c-string from the time stamp of the frame.  Note:
//  for older files, the time may be in error and it can be better
//  to use the year, month, day, hour, min, sec in the header
//  instead of the timestamp.  The method used below is compatible
//  with ARIS as well as more recent DIDSON formats and was therefore
//  prefered.  Use the commented out line instead for old formats.
//-----------------------------------------------------------
char * DIDSONFrame::getDateTimeString(){
    char *buf = new char[21];
    QString dateString = frameTime.toString(QString("yyyy/MM/dd hh:mm:ss"));
    QByteArray ba = dateString.toLocal8Bit();
    buf = ba.data();

    return buf;
}

QDateTime DIDSONFrame::getDateTime(){
    return frameTime;
}

void DIDSONFrame::setAvgBeams(int value){
    avgBeams = value;
}

void DIDSONFrame::setDecibelCorrection(float value){
    decibelCorrection = value;
}

void DIDSONFrame::setFirstFrame(){
    firstFrame = currentFrameNumber;
}

void DIDSONFrame::setTVG(float value){
    tvg = value;
    calculateTVGValues();
}

void DIDSONFrame::setAlpha(float value){
    alpha = value;
    calculateTVGValues();
}



