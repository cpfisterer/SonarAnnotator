/*
 *  SonarData.cpp
 *  iSonar
 *
 *  Created by Carl Pfisterer on 6/4/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include "sonardata.h"

SonarData::SonarData(){
    manufacturer = new char[15];
    if(manufacturer == nullptr){
        printf("Problem allocating manufacturer array");
    }
    file = nullptr;
    amplitude = nullptr;
    horizontal = nullptr;
    vertical = nullptr;
    pingArray = nullptr;
    backgroundValues = nullptr;
    stdDevValues = nullptr;
    rotate = true;
    flip = false;
    minRange = 0;
    tvg = 40.0;
    alpha = 0.0;
    numberToAverage = 200;

    TVGValues = new float[500*100];		//TVG extends to 500 meters with a resolution of .01 meters.
    setTVGValues();
    progress = new QProgressDialog("Reading File",QString(),0,0,nullptr);
    progress->setWindowModality(Qt::WindowModal);
    progress->reset();
}

SonarData::~SonarData(){
    delete [] manufacturer;
    closeFile();
    clearData();
    delete progress;
    qDebug() << "Sonar Data deallocated";
}

//----------------------------------------------------------------------------------
//  calculateBackground()
//  Creates a matrix of background values for background removal or subtraction.
//  Breaks the file into 10 sections and calculates the average amplitude across all
//  the pings in each section.  Thus the size of the matrix is numberOfSamples*10.
//  This uses an algorithm to calculate both the mean and standard deviation in one
//  loop rather than two.  This speeds up this function tremendously and was found at
//  http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance from the reference
//  to Welford.
//----------------------------------------------------------------------------------
void SonarData::calculateBackground(){
    int i,j,k,n = 10,n2=0,index;
    int avgNum = int(fmin(numberOfPings/n,numberToAverage));
    float value,correction = 0.0;
    float mean,M2,delta;

    if(amplitude){              //Check to make sure there is data to average
        if(backgroundValues){
            delete [] backgroundValues;
        }
        if(stdDevValues){
            delete [] stdDevValues;
        }
        backgroundValues = new float[numberOfSamples*10];
        stdDevValues = new float [numberOfSamples*10];
        for(i = 0; i < n; i++){
            for(j = 0; j < numberOfSamples; j++){
                n2 = 0; mean = 0; M2 = 0;
                correction = TVGCompensation(j);
                index = min(10*numberOfSamples-1,i*numberOfSamples+j);
                for(k = 0; k < avgNum; k++){
                    value = getDecibel(amplitude[min(numberOfPings*numberOfSamples-1,i*(numberOfPings/n)*numberOfSamples+k*numberOfSamples+j)])+correction;
                    n2 = n2 + 1;
                    delta = value - mean;
                    mean = mean + delta/float(n2);
                    M2 = M2 + delta*(value - mean);
                }
                backgroundValues[index] = mean;
                stdDevValues[index] = pow(M2/float(n2-1),0.5f);
            }
        }
    }
}

//----------------------------------------------------------------------------------
// checkRectangle()
// Checks the bounds of the rectangle to ensure they don't exceed the dimensions
// of the data.
//----------------------------------------------------------------------------------
Rectangle SonarData::checkRectangle(Rectangle rect){
    Rectangle newRect;

    newRect.x = rect.x<0 ? 0:rect.x;
    newRect.y = rect.y<0 ? 0:rect.y;
    newRect.w = rect.x+rect.w > numberOfSamples-1 ? numberOfSamples-1:numberOfSamples-1-rect.x;
    newRect.h = rect.y+rect.h > numberOfPings-1 ? numberOfPings-1:numberOfPings-1-rect.y;

    return newRect;
}

//----------------------------------------------------------------------------------
//  clearData()
//  Clears any existing data.
//----------------------------------------------------------------------------------
void SonarData::clearData(){
    if(amplitude){
        delete [] amplitude;
        amplitude = nullptr;
    }

    if(horizontal){
        delete [] horizontal;
        horizontal = nullptr;
    }

    if(vertical){
        delete [] vertical;
        vertical = nullptr;
    }

    if(backgroundValues){
        delete [] backgroundValues;
        backgroundValues = nullptr;
    }

    if(stdDevValues){
        delete [] stdDevValues;
        stdDevValues = nullptr;
    }
}

//----------------------------------------------------------------------------------
//  closeFile()
//  Closes the file for reading.
//----------------------------------------------------------------------------------
void SonarData::closeFile(){
    if(file){
        fclose(file);
        file = nullptr;
    }
}

//----------------------------------------------------------------------------------
//  exportData()
//  Exports Data between the pings/frames specified.
//----------------------------------------------------------------------------------
void SonarData::exportData(int start, int end, char *saveName){
    int length = pingArray->get(end)->fileLocation-pingArray->get(start)->fileLocation;
    char *buffer = new char[length];
    FILE *sFile;

    if((sFile=fopen(saveName,"wb"))==nullptr){
        printf("Can't open %s\n",saveName);
        qDebug() << "Can't open" << saveName;
        return;
    }
    fseek(file,0,SEEK_SET);
    progress->setMaximum(0);
    progress->setLabelText(QString("Exporting File Header"));
    progress->show();
    exportHeader(sFile,end-start);
    fseek(file,pingArray->get(start)->fileLocation,SEEK_SET);
    progress->setLabelText(QString("Exporting Data"));
    fread(buffer,sizeof(char),ulong(length),file);     //  Read from open file
    fwrite(buffer,sizeof(char),ulong(length),sFile);   //  Write to save file
    fclose(sFile);
    delete [] buffer;
    progress->reset();
}

void SonarData::exportHeader(__attribute__((unused)) FILE *saveFile,__attribute__((unused)) int number){

}

void SonarData::exportPings(int start,int end,char *saveName){
    FILE *file;

    start = int(fmin(fmax(0,start),numberOfPings));
    end = int(fmax(0,fmin(end,numberOfPings)));
    qDebug() << "Exporting Pings" << start << "to" << end << "  Total number of samples=" << numberOfSamples << "  Samples per meter=" << samplesPerMeter;
    if((file=fopen(saveName,"wb"))==nullptr){
        printf("Can't open %s\n",saveName);
        qDebug() << "Can't open" << saveName;
        return;
    }

    fprintf(file,"Amplitude\n");
    for(int p = start; p <= end; p++){
        for(int i = 0; i < numberOfSamples; i++){
            fprintf(file,"%f,",double(this->getDecibel(i,p)));
        }
        fprintf(file,"\n");
    }
    fprintf(file,"\nHorizontal\n");
    for(int p = start; p <= end; p++){
        for(int i = 0; i < numberOfSamples; i++){
            fprintf(file,"%f,",double(this->getHorizontalAngle(i,p)));
        }
        fprintf(file,"\n");
    }
    fprintf(file,"\nVertical\n");
    for(int p = start; p <= end; p++){
        for(int i = 0; i < numberOfSamples; i++){
            fprintf(file,"%f,",double(this->getVerticalAngle(i,p)));
        }
        fprintf(file,"\n");
    }
    fprintf(file,"\n");
}

//----------------------------------------------------------------------------------
//  fromLittleEndian()
//  Converts little endian values to big the native format.
//  Note: Uses Qt specific call qFromLittleEndian
//----------------------------------------------------------------------------------
short SonarData::fromLittleEndian(short value){
    return short(qFromLittleEndian(qint16(value)));
}

//----------------------------------------------------------------------------------
//  fromLittleEndian()
//  Converts little endian values to big the native format.  This function is
//  overriden for the data type used.
//  Note: Uses Qt specific call qFromLittleEndian
//----------------------------------------------------------------------------------
int SonarData::fromLittleEndian(int value){
    return int(qFromLittleEndian(qint32(value)));
}

//----------------------------------------------------------------------------------
//  fromLittleEndian()
//  Converts little endian values to big the native format.  This function is
//  overriden for the data type used.
//  Note: Uses Qt specific call qFromLittleEndian
//----------------------------------------------------------------------------------
long long SonarData::fromLittleEndian(long long value){
    return int64_t(qFromLittleEndian(qint64(value)));
}

//----------------------------------------------------------------------------------
//  fromLittleEndian()
//  Converts little endian values to big the native format.  This function is
//  overriden for the data type used.
//  Note: Uses Qt specific call qFromLittleEndian
//----------------------------------------------------------------------------------
float SonarData::fromLittleEndian(float value){
    float *value2;
    qint32 newValue;

    value2 = &value;
    newValue = qFromLittleEndian(qint32(*(reinterpret_cast<int*>(value2))));
    value2 = reinterpret_cast<float*>(&newValue);

    return *value2;
}

//----------------------------------------------------------------------------------
//  fromLittleEndian()
//  Converts little endian values to big the native format.  This function is
//  overriden for the data type used.
//  Note: Uses Qt specific call qFromLittleEndian
//----------------------------------------------------------------------------------
double SonarData::fromLittleEndian(double value){
    double *value2;
    qint64 newValue;

    value2 = &value;
    newValue = qFromLittleEndian(qint64(*(reinterpret_cast<long long*>(value2))));
    value2 = reinterpret_cast<double*>(&newValue);

    return *value2;
}

//----------------------------------------------------------------------------------
//  indexFile()
//  Virtual function to be overridden.  Index file and return index array.
//----------------------------------------------------------------------------------
PingList* SonarData::indexFile(FILE* file){
    if(file){}
    return nullptr;
}

//----------------------------------------------------------------------------------
// getAngle()
// Receives the angle coded to a signed short value and returns the angle in
// degrees.
//----------------------------------------------------------------------------------
float SonarData::getAngle(signed short value){
    float angle = float(value)/1000.0f;

    return angle;
}

//----------------------------------------------------------------------------------
//  getBackground()
//  Return the background values.
//----------------------------------------------------------------------------------
float* SonarData::getBackground(){
    return backgroundValues;
}

//----------------------------------------------------------------------------------
// getDecibel()
// Returns the decibel value from the amplitude values that are coded into
// unsigned short values;
//----------------------------------------------------------------------------------
float SonarData::getDecibel(unsigned short value){
    float decibel = float(value/-300.0f);

    return decibel;
}

//----------------------------------------------------------------------------------
//  getDecibel()
//  Overloaded function returns the tvg adjusted decibel value for sample and ping location.
//----------------------------------------------------------------------------------
float SonarData::getDecibel(int sample, int ping){
    sample = int(fmin(numberOfSamples,sample));
    ping = int(fmin(numberOfPings,ping));
    float correction = TVGCompensation(sample);
    return (getDecibel(amplitude[min(numberOfSamples*numberOfPings-1,ping*numberOfSamples+sample)])+correction);
}

//----------------------------------------------------------------------------------
//  getDecibels()
//  Returns the decibel values within the rectangle specified.  This function
//  allocates memory and returns a pointer to the data.  The pointers are not freed
//  in this class.  They will need to be released by the calling routines!!!
//  --updated 4/10/18 to make blank any data not within range bounds
//----------------------------------------------------------------------------------
float* SonarData::getDecibels(Rectangle rect, float minMarkRange, float maxMarkRange){
    float* values = new float[rect.w*rect.h];
    float a, correction=0;

    int i,j,width,height,x,y;

    if(rotate){width = rect.h;height = rect.w;x = rect.y;y = rect.x;}
    else {width = rect.w;height = rect.h;x = rect.x;y = rect.y;}

    for(i = 0; i < width; i++){
        if(flip) correction = TVGCompensation(numberOfSamples-(x+i));
        else correction = TVGCompensation(i+x);
        for(j = 0; j < height; j++){
            if((float(i+x)/samplesPerMeter+minRange)>=minMarkRange && (float(i+x)/samplesPerMeter+minRange)<=maxMarkRange){
                //a = float(amplitude[(j+y)*numberOfSamples+(i+x)]/-300.0f+correction);           //No bounds checks
                a = float(amplitude[(j+y)*numberOfSamples+(i+x)]+correction);           //No bounds checks
            }
            else {
                a = 0;
            }
            values[j*rect.w*(!rotate)+i*(!rotate)+i*rect.w*rotate+j*rotate] = a;    //No bounds checks
        }        
    }
    return values;
}

//----------------------------------------------------------------------------------
//  getEndTime()
//  Returns the time of the last ping.
//----------------------------------------------------------------------------------
QDateTime SonarData::getEndTime(){
    return endTime;
}

//----------------------------------------------------------------------------------
//  getHorizontal()
//  Returns the horizontal angle values within the rectangle specified.  This function
//  allocates memory and returns a pointer to the data.  The pointers are not freed
//  in this class.  They will need to be released by the calling routines!!!
//----------------------------------------------------------------------------------
float* SonarData::getHorizontal(Rectangle rect){
    float* values = new float[rect.w*rect.h];
    float a;
    int i,j;

    for(i = 0; i < rect.w; i++){
        for(j = 0; j < rect.h; j++){
            if(rotate && flip){
                a = getAngle(horizontal[min(numberOfSamples*numberOfPings-1,((rect.x+i)*numberOfSamples+(numberOfSamples-(rect.y+j))))]);
            }
            else if(rotate && !flip){
                a = getAngle(horizontal[min(numberOfSamples*numberOfPings-1,((rect.x+i)*numberOfSamples+(rect.y+j)))]);
            }
            else if(!rotate && flip){
                a = getAngle(horizontal[min(numberOfSamples*numberOfPings-1,(j+rect.y)*numberOfSamples+(numberOfSamples-(i+rect.x)))]);
            }
            else{
                a = getAngle(horizontal[min(numberOfSamples*numberOfPings-1,(j+rect.y)*numberOfSamples+(i+rect.x))]);
            }
            values[min(rect.w*rect.h-1,j*rect.w+i)] = a;
        }
    }

    return values;
}

//----------------------------------------------------------------------------------
//  getHorizontalAngle()
//  Returns the horizontal angle for sample and ping location
//----------------------------------------------------------------------------------
float SonarData::getHorizontalAngle(int sample,int ping){
    return getAngle(horizontal[min(numberOfSamples*numberOfPings-1,ping*numberOfSamples+sample)]);
}

//----------------------------------------------------------------------------------
//  getHorizontalBeamWidth()
//  Returns the width of the beam in the x dimension.
//----------------------------------------------------------------------------------
float SonarData::getHorizontalBeamWidth(){
    return hBeamWidth;
}

//----------------------------------------------------------------------------------
//  getManufacturer()
//  Returns the string containing the name of the manufacturer.
//----------------------------------------------------------------------------------
char* SonarData::getManufacturer(){
    return manufacturer;
}

//----------------------------------------------------------------------------------
//  getMaxRange()
//  Returns the maximum range of the dataset.
//----------------------------------------------------------------------------------
float SonarData::getMaxRange(){
    return maxRange;
}

//----------------------------------------------------------------------------------
//  getMinRange()
//  Return the minimum range of the dataset.
//----------------------------------------------------------------------------------
float SonarData::getMinRange(){
    return minRange;
}

//----------------------------------------------------------------------------------
//  getNumberOfPings()
//  Returns the total number of pings.
//----------------------------------------------------------------------------------
int SonarData::getNumberOfPings(){
    return numberOfPings;
}

//----------------------------------------------------------------------------------
//  getNumberOfSamples()
//  Returns the number of samples per ping.
//----------------------------------------------------------------------------------
int SonarData::getNumberOfSamples(){
    return numberOfSamples;
}

//----------------------------------------------------------------------------------
//  getPingRate()
//  Returns the ping rate (or frame rate for DIDSON data).
//----------------------------------------------------------------------------------
float SonarData::getPingRate(){
    return pingRate;
}

//----------------------------------------------------------------------------------
//  getRange()
//  Calculates and returns the range for a given sample number - based on sample rate
//----------------------------------------------------------------------------------
float SonarData::getRange(int sample){
    return minRange+float(sample)/samplesPerMeter;
}

//----------------------------------------------------------------------------------
//  getSamplesPerMeter()
//  Returns the digital sample rate
//----------------------------------------------------------------------------------
float SonarData::getSamplesPerMeter(){
    return samplesPerMeter;
}

//----------------------------------------------------------------------------------
//  getStartTime()
//  Returns the time of the first ping.
//----------------------------------------------------------------------------------
QDateTime SonarData::getStartTime(){
    return startTime;
}

//----------------------------------------------------------------------------------
//  getStdDevValues()
//  Returns a pointer to the standard deviation values used for background removal.
//----------------------------------------------------------------------------------
float* SonarData::getStdDevValues(){
    return stdDevValues;
}

//----------------------------------------------------------------------------------
//  getTime()
//  Calculates and returns the elapsed time for a given ping number - based on ping rate
//----------------------------------------------------------------------------------
float SonarData::getTime(int ping){
    return float(ping)/pingRate/60;
}

//----------------------------------------------------------------------------------
//  getVertical()
//  Returns the vertical angle values within the rectangle specified.  This function
//  allocates memory and returns a pointer to the data.  The pointers are not freed
//  in this class.  They will need to be released by the calling routines!!!
//----------------------------------------------------------------------------------
float* SonarData::getVertical(Rectangle rect){
    float* values = new float[rect.w*rect.h];
    float a;
    int i,j;

    for(i = 0; i < rect.w; i++){
        for(j = 0; j < rect.h; j++){
            if(rotate && flip){
                a = getAngle(vertical[min(numberOfSamples*numberOfPings-1,((rect.x+i)*numberOfSamples+(numberOfSamples-(rect.y+j))))]);
            }
            else if(rotate && !flip){
                a = getAngle(vertical[min(numberOfSamples*numberOfPings-1,((rect.x+i)*numberOfSamples+(rect.y+j)))]);
            }
            else if(!rotate && flip){
                a = getAngle(vertical[min(numberOfSamples*numberOfPings-1,(j+rect.y)*numberOfSamples+(numberOfSamples-(i+rect.x)))]);
            }
            else{
                a = getAngle(vertical[min(numberOfSamples*numberOfPings-1,(j+rect.y)*numberOfSamples+(i+rect.x))]);
            }
            values[min(rect.w*rect.h-1,j*rect.w+i)] = a;
        }
    }

    return values;
}

//----------------------------------------------------------------------------------
//  getVerticalAngle()
//  Overloaded function returns the vertical angle for sample and ping location
//----------------------------------------------------------------------------------
float SonarData::getVerticalAngle(int sample,int ping){
    return getAngle(vertical[min(numberOfSamples*numberOfPings-1,ping*numberOfSamples+sample)]);
}

//----------------------------------------------------------------------------------
//  getVerticalBeamWidth()
//  Returns the width of the beam in the y dimension.
//----------------------------------------------------------------------------------
float SonarData::getVerticalBeamWidth(){
    return vBeamWidth;
}

//----------------------------------------------------------------------------------
// getVoltage()
// Takes a decibel value and returns the voltage equivalent
//----------------------------------------------------------------------------------
float SonarData::getVoltage(float value){
    return pow(10.0f,value/20.0f);
}

//----------------------------------------------------------------------------------
//  getVoltage()
//  Overloaded function to retrieive voltage for a sample and ping location
//----------------------------------------------------------------------------------
float SonarData::getVoltage(int sample, int ping){
    return getVoltage(getDecibel(sample,ping));
}

//----------------------------------------------------------------------------------
//  getVoltages()
//  Returns the voltage values within the rectangle specified.  This function
//  allocates memory and returns a pointer to the data.  The pointers are not freed
//  in this class.  They will need to be released by the calling routines!!!
//----------------------------------------------------------------------------------
float * SonarData::getVoltages(Rectangle rect){
    float* values = new float[rect.w*rect.h];
    float a,correction;
    int i,j;

    for(i = 0; i < rect.w; i++){
        for(j = 0; j < rect.h; j++){
            if(rotate && flip){
                correction = TVGCompensation(numberOfSamples-(rect.y+j));
                a = getDecibel(amplitude[((rect.x+i)*numberOfSamples+(numberOfSamples-(rect.y+j)))])+correction;
                a = getVoltage(a);
            }
            else if(rotate && !flip){
                correction = TVGCompensation(rect.y+j);
                a = getDecibel(amplitude[((rect.x+i)*numberOfSamples+(rect.y+j))])+correction;
                a = getVoltage(a);
            }
            else if(!rotate && flip){
                correction = TVGCompensation(numberOfSamples-(i+rect.x));
                a = getDecibel(amplitude[(j+rect.y)*numberOfSamples+(numberOfSamples-(i+rect.x))])+correction;
                a = getVoltage(a);
            }
            else{
                correction = TVGCompensation(i+rect.x);
                a = getDecibel(amplitude[(j+rect.y)*numberOfSamples+(i+rect.x)])+correction;
                a = getVoltage(a);
            }
            values[j*rect.w+i] = a;
        }
    }
    return values;
}

//----------------------------------------------------------------------------------
//  openFile()
//  Opens the binary file for reading, reads configuration, then indexes the location
//  in the file for the start of each ping, clears any old data, then reads data.
//  Also calculates the background.
//----------------------------------------------------------------------------------
bool SonarData::openFile(char *filename){
    if((file=fopen(filename,"rb"))==nullptr){
        printf("Can't open %s\n",filename);
        qDebug() << "Can't open" << filename;
        return false;
    }
    emit appendText(tr("Manufacturer: %1").arg(manufacturer));
    readConfiguration(file);
    if(pingArray) delete pingArray;
    progress->setMaximum(0);
    progress->setLabelText("Indexing File");
    progress->show();
    pingArray = indexFile(file);
    numberOfPings = pingArray->count();
    if(numberOfPings > 0){
        numberOfSamples = pingArray->numberOfSamples();
        maxRange = float(numberOfSamples)/samplesPerMeter;
        clearData();
        progress->setMaximum(numberOfPings);
        progress->setLabelText(tr("Reading File: %1").arg(filename));
        readData(file);
        progress->setMaximum(0);
        progress->setLabelText("Computing Background");
        calculateBackground();
        progress->reset();

        return true;
    }
    else{
        return false;
    }
}

//----------------------------------------------------------------------------------
//  readAmplitude()
//  Retrieves the ampltude values for the ping passed.
//----------------------------------------------------------------------------------
float* SonarData::readAmplitude(int ping){
    float* values = new float[numberOfSamples];
    int i;

    for(i = 0; i < numberOfSamples; i++){
        values[i] = amplitude[min(numberOfPings*numberOfSamples-1,ping*numberOfSamples+i)];
    }

    return values;
}

//----------------------------------------------------------------------------------
//  readBuffer()
//  Reads a character buffer starting at the current file location into the passed
//  buffer.
//----------------------------------------------------------------------------------
void SonarData::readBuffer(FILE* file,char* buffer,int length){
    fread(buffer,sizeof(char),ulong(length),file);
}

//----------------------------------------------------------------------------------
//  readChar()
//  reads a character value from the current file location and returns the value.
//----------------------------------------------------------------------------------
char SonarData::readChar(FILE* file){
    char value;
    fread(&value,sizeof(char),1,file);

    return value;
}

//----------------------------------------------------------------------------------
//  readConfiguration()
//  Reads the system configuration from the file.  Meant to be overridden.
//----------------------------------------------------------------------------------

void SonarData::readConfiguration(FILE* file){
    if(file){}
}

//----------------------------------------------------------------------------------
//  readData()
//  Read the data from file.  To be overridden.
//----------------------------------------------------------------------------------
void SonarData::readData(FILE* file){
    if(file){}
}

//----------------------------------------------------------------------------------
//  readDouble()
//  Reads and returns the double value at the current file location.
//  Reverses byte order on big endian processors.
//----------------------------------------------------------------------------------
double SonarData::readDouble(FILE* file){
    double value;

    fread(&value, sizeof(double), 1, file);

    return SonarData::fromLittleEndian(value);
}

//----------------------------------------------------------------------------------
//  readFloat()
//  Reads and returns the floating point value at the current file location.
//  Reverses byte order on big endian processors.
//----------------------------------------------------------------------------------
float SonarData::readFloat(FILE* file){
    float value;

    fread(&value, sizeof(float), 1, file);
    value = SonarData::fromLittleEndian(value);

    return value;
}

//----------------------------------------------------------------------------------
//  readInt()
//  Reads and returns the integer value at the current file location.
//  Reverses byte order on big endian processors.
//----------------------------------------------------------------------------------
int SonarData::readInt(FILE* file){
    int value;

    fread(&value, sizeof(int), 1, file);

    return SonarData::fromLittleEndian(value);
}

//----------------------------------------------------------------------------------
//  readLong()
//  Reads and returns the long integer value at the current file location.
//  Reverses byte order on big endian processors.
//----------------------------------------------------------------------------------
long long SonarData::readLong(FILE* file){
    long long value;

    fread(&value, sizeof(long long), 1, file);

    return SonarData::fromLittleEndian(value);
}

//----------------------------------------------------------------------------------
//  readShort()
//  Reads and returns the short integer value at the current file location.
//  Reverses byte order on big endian processors.
//----------------------------------------------------------------------------------
short SonarData::readShort(FILE* file){
    short value;

    fread(&value, sizeof(short), 1, file);

    return SonarData::fromLittleEndian(value);

}

//----------------------------------------------------------------------------------
//  setAlpha()
//  Sets the attenuation coefficient alpha then recalculates the time varied gain.
//----------------------------------------------------------------------------------
void SonarData::setAlpha(float value){
    alpha = value;
    setTVGValues();
}

//----------------------------------------------------------------------------------
//  setFlip()
//  Sets the flip range flag.
//----------------------------------------------------------------------------------
void SonarData::setFlip(bool doFlip){
    flip = doFlip;
}

//----------------------------------------------------------------------------------
//  setNumberToAverage()
//  Sets the number of pings to use when averaging the background for
//  bottom removal.
//----------------------------------------------------------------------------------
void SonarData::setNumberToAverage(int value){
    numberToAverage = value;
}

//----------------------------------------------------------------------------------
//  setRotate()
//  Sets the rotation flag
//----------------------------------------------------------------------------------
void SonarData::setRotate(bool doRotate){
    rotate = doRotate;
}

//----------------------------------------------------------------------------------
//  setTVG()
//  Sets the spherical spreading coefficient, then recalculates the time varied gain.
//----------------------------------------------------------------------------------
void SonarData::setTVG(float value){
    tvg = value;
    setTVGValues();
    //calculateBackground();
}

//----------------------------------------------------------------------------------
//  setTVGValues()
//  Computes the array of TVG compensation values based on the current alpha and
//  TVG multiplier.
//----------------------------------------------------------------------------------
void SonarData::setTVGValues(){
    int i;
    float r;

    for(i = 1; i < 500*100+1; i++){		//Initialize TVG array
        r = float(i)/100.0f;
        TVGValues[i-1] = float(fmax(0,tvg*log10(r)+2*alpha*r));
    }
}

//----------------------------------------------------------------------------------
// TVGCompensation()
// Returns the TVG compensation for the range at the sample
//----------------------------------------------------------------------------------
float SonarData::TVGCompensation(int sample){
    float range;

    range = fabs(float(sample)/samplesPerMeter+minRange);

    return TVGValues[int(range*100)];
}

