#include "arisframe.h"

/*ArisFrame::ArisFrame()
{
}*/

ArisFrame::ArisFrame(FILE *newFile, int numBeams, int res, int ver):DIDSONFrame(){
    file = newFile;
    numberOfSamples = 0;
    numberOfBeams = numBeams;
    previousNumberOfBeams = numberOfBeams;
    frame = new float[numberOfSamples*numberOfBeams];
    resolution = res;
    alpha = 0.0;
    tvg = 0.0;
    decibelCorrection = -100;
    avgBeams = 4;
    tvgValues = nullptr;
    previousNumberOfSamples = numberOfSamples;
    version = ver;
    fileHeaderLength=1024;
    frameHeaderLength=1024;
}


QString ArisFrame::getSystemType(){
    if(systemType==0){
        return QString("ARIS 1800");
    }
    else if(systemType==1){
        return QString("ARIS 3000");
    }
    return QString("ARIS 1200");
}

void ArisFrame::readFrameHeader(){
    int tempInt,configFlag;
    float tempFloat;
    long long tempLong;
    int64_t currentOffset;
    double tempDouble;

    currentOffset=int64_t(ftello(file));
    currentFrameNumber=SonarData::readInt(file);			//Frame number
    tempLong=SonarData::readLong(file);                     //Date/time value
    frameTime.setTime_t(uint(tempLong/1000000)); //Convert to number of seconds since 1/1/1970
    tempInt = SonarData::readInt(file);                     //Version
    tempInt=SonarData::readInt(file);                       //Status
    tempLong = SonarData::readLong(file);                   //Sonar time stamp
    tempInt=SonarData::readInt(file);                       //Day
    tempInt=SonarData::readInt(file);                       //Hour
    tempInt=SonarData::readInt(file);                    	//Minute
    tempInt=SonarData::readInt(file);                    	//Second
    tempInt=SonarData::readInt(file);                   	//Hundreths of second
    tempInt=SonarData::readInt(file);                   	//Transmitt mode
    windowStart=SonarData::readFloat(file);					//Start Range
    windowLength=SonarData::readFloat(file);				//End Range
    tempInt=SonarData::readInt(file);                       //Threshold
    tempInt=SonarData::readInt(file);                       //Intensity
    gain=SonarData::readInt(file);                          //Gain
    tempInt=SonarData::readInt(file);                   	//Deg C #1
    tempInt=SonarData::readInt(file);                   	//Deg C #2
    humidity=SonarData::readInt(file);                   	//Humidity
    focus=float(SonarData::readInt(file));                  //Focus
    power=SonarData::readInt(file);             			//Battery
    fseeko(file,int64_t(16),SEEK_CUR);                      //Status 1
    fseeko(file,int64_t(16),SEEK_CUR);                      //Status 2
    tempFloat=SonarData::readFloat(file);					//Velocity
    tempFloat=SonarData::readFloat(file);					//Depth
    tempFloat=SonarData::readFloat(file);					//Altitude
    tempFloat=SonarData::readFloat(file);					//Pitch
    tempFloat=SonarData::readFloat(file);					//Pitch rate
    tempFloat=SonarData::readFloat(file);					//Roll
    tempFloat=SonarData::readFloat(file);					//Roll rate
    tempFloat=SonarData::readFloat(file);					//Heading
    tempFloat=SonarData::readFloat(file);					//Heading rate
    pan=SonarData::readFloat(file);                         //Compass pan
    tilt=SonarData::readFloat(file);                        //Compass tilt
    roll=SonarData::readFloat(file);                        //Compass roll
    tempDouble=SonarData::readDouble(file);                 //Latitude
    tempDouble=SonarData::readDouble(file);             	//Longitude
    tempFloat=SonarData::readFloat(file);					//Sonar position
    configFlag=SonarData::readInt(file);					//Config flag
    configFlag = configFlag&0x0000000f;                     //Only use the last bit
    fseeko(file,int64_t(28),SEEK_CUR);                                //Skip 28 bytes
    temperature = SonarData::readFloat(file);               //Temperature from housing
    fseeko(file,int64_t(192),SEEK_CUR);                               //Skip 192 bytes
    sampleRate = SonarData::readFloat(file);                //Sample rate in Hz???
    tempFloat = SonarData::readFloat(file);                 //Accell X
    tempFloat = SonarData::readFloat(file);                 //Accell Y
    tempFloat = SonarData::readFloat(file);                 //Accell Z
    pingMode = SonarData::readInt(file);                    //ARIS ping mode [1...12]
    frequencyHiLo = SonarData::readInt(file);               //1 = HF, 0 = LF
    pulseWidth = SonarData::readInt(file);                  //Pulse width in usec [4..100]
    cyclePeriod = SonarData::readInt(file);                 //Ping cycle time in usec [1802..65535]
    samplePeriod = SonarData::readInt(file);                //Downrange samplerate in usec [4..100]
    tempInt = SonarData::readInt(file);                     //Tranmitt enabled, 1 = ON, 0 = OFF
    tempFloat = SonarData::readFloat(file);                 //Instantaneous frame rate between frame N and N-1
    c = SonarData::readFloat(file);                         //Speed of sound
    numberOfSamples = SonarData::readInt(file);             //Number of downrange samples in each beam
    tempInt = SonarData::readInt(file);                     //1 = 150 V ON (max power), 0 = 150 V OFF (min power 12V)
    sampleStartDelay = SonarData::readInt(file);            //Delay from start of transmit until start of sample (window start) in usec [930...65535]
    largeLens = SonarData::readInt(file);                   //1 = telephoto lens (large lens, big lens, hi-res lens) present
    systemType = SonarData::readInt(file);                  //0 = ARIS 1800, 1 = ARIS 3000, 2 = ARIS 1200
    serialNumber = SonarData::readInt(file);                //Sonar serial number as labled on housing
    tempLong = SonarData::readLong(file);               //Encryption key (64 bit int)
    tempInt = SonarData::readInt(file);                     //ARIS error flag
    tempInt = SonarData::readInt(file);                     //Missed packet count;
    appVersion = SonarData::readInt(file);                  //Version number of ARIS application sending frame
    tempInt = SonarData::readInt(file);                     //Reserved for future use
    reorderedSamples = SonarData::readInt(file);            //1 = frame data ordered into [beam,sample], 0 = needs reordering
    salinity = SonarData::readInt(file);                    //0 = fresh, 15 = brackish, 30 = salt
    pressure = SonarData::readFloat(file);                    //Depth sensor output in meters

    fseeko(file,int64_t(frameHeaderLength) - int64_t(ftello(file) - int64_t(currentOffset)),SEEK_CUR);   //Skip to the end of the frame header
    windowStart = float(sampleStartDelay) * float(double(c)/2000000.0);
    windowLength = float(samplePeriod*numberOfSamples)*float(double(c)/2000000.0);

    if((numberOfSamples != previousNumberOfSamples)|| (numberOfBeams != previousNumberOfBeams)){
        if(tvgValues){
            delete [] tvgValues;
        }
        tvgValues = new float[numberOfSamples+2];
        calculateTVGValues();
        if(frame){
            delete [] frame;
        }
        frame = new float[numberOfSamples*numberOfBeams];
        previousNumberOfSamples = numberOfSamples;
    }
    frameLength = frameHeaderLength+numberOfSamples*numberOfBeams;
}

void ArisFrame::readFrame(){
    this->readFrameNoTVG();
}

void ArisFrame::readFrameNoTVG(){
    readFrameHeader();
    unsigned char data[numberOfSamples*numberOfBeams];
    SonarData::readBuffer(file,reinterpret_cast<char*>(data),int(sizeof(unsigned char)*ulong(numberOfSamples*numberOfBeams)));
    data[0] = 0;    //Set first value to zero for background
    correctAmplitude(data);
}

bool ArisFrame::getLargeLens(){
    if(largeLens==1){
        return true;
    }
    return false;
}

float ArisFrame::getSpeedOfSound(){
    return c;
}

void ArisFrame::goToFrame(int frameNumber){
    int result;
    int64_t test = int64_t(fileHeaderLength)+(int64_t(frameNumber)*int64_t(frameLength));
    result = fseeko(file,off_t(test),SEEK_SET);

    readFrame();
}
