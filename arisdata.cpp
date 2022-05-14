#include "arisdata.h"

ArisData::ArisData():DIDSONData()
{
    strcpy(manufacturer,"SMC/ARIS");
}

void ArisData::readConfiguration(FILE *filename){
    char buffer[5];

    this->readBuffer(filename,buffer,3);buffer[3] = '\0';
    version = int(this->readChar(filename));
    emit appendText(tr("File Version: %1%2").arg(buffer).arg(version));
    if(version == 5){
        fileHeaderLength = 1024;
        frameHeaderLength = 1024;
        this->readConfigurationV5(filename);
    }
    else {
        printf("Version not recognized\n");
    }//Error
}

void ArisData::readConfigurationV5(FILE *filename){
    int intValue;
    long long longValue;

    numberOfPings = readInt(filename); emit appendText(tr("Number of Frames: %1").arg(numberOfPings));   //	Number of pings
    pingRate = readInt(filename); emit appendText(tr("Frame Rate: %1").arg(double(pingRate)));                   //	Ping rate
    hResolution = readInt(filename);                                                                     //	Horizontal resolution flag
    if(hResolution == 0) {hResolution = 1;emit appendText(tr("Low Resolution"));}
    else {hResolution = 0;emit appendText(tr("High Resolution"));}
    numberOfBeams = readInt(filename); emit appendText(tr("Number of Beams: %1").arg(numberOfBeams));   //  Number of beams
    intValue = readInt(filename);                                                                       //  Sample rate
    numberOfSamples = readInt(filename);                                                                //	Samples per channel
    emit appendText(tr("Number of samples: %1").arg(numberOfSamples));
    intValue = readInt(filename);                                                                       //	Gain
    Gr = float(intValue); emit appendText(tr("Gain: %1").arg(double(Gr)));
    minRange = readFloat(filename);                                                                     //	Start range
    maxRange = readFloat(filename);                                                                     //	Window length flag
    intValue = readInt(filename);                                                                       //	Reverse
    if(intValue == 1){reverse = true;emit appendText(tr("Reverse: True"));}
    else{reverse = false;emit appendText(tr("Reverse: False"));}
    serialNumber=readInt(filename);	appendText(tr("Serial Number: %1").arg(serialNumber));          	//	Serial number
    fseeko(filename,32,SEEK_CUR);                                                                        //	Date
    fseeko(filename,256,SEEK_CUR);                                                                       //	User defined
    fseeko(filename,48,SEEK_CUR);                                                                        //  Skip DIDSON specific values
    fseeko(filename,28,SEEK_CUR);                                                                        //  Skip DIDSON specific values
    intValue = readInt(filename);                                                                       //  Version FPGA reserved for future use
    intValue = readInt(filename);                                                                       //  Version PSuC reserved for future use
    intValue = readInt(filename);                                                                       //  Thumbnail frame number
    longValue = readLong(filename);                                                                     //  Total file size in bytes
    longValue = readLong(filename);                                                                     //  Optional header size reserved for future use
    longValue = readLong(filename);                                                                     //  Optional tail size reserved for future use
    intValue = readInt(filename);                                                                       //  DIDSON adjusted version minor
    intValue = readInt(filename);                                                                       //  non-zero if telephoto (large/big) lens present
    fseeko(filename,fileHeaderLength,SEEK_SET);
    if(frame){delete frame;}
    frame = new ArisFrame(filename,numberOfBeams,hResolution,version);
    numberOfBeamsToAverage = int(fmin(numberOfBeamsToAverage,numberOfBeams));                            //Ensure we don't average across more beams than present
    frame->setAvgBeams(numberOfBeamsToAverage);
    frame->setDecibelCorrection(didsonCorrection);
    frame->goToFrame(0);
    maxRange = frame->getMaximumRange();
    minRange = frame->getWindowStart();
    bigLens = dynamic_cast<ArisFrame*>(frame)->getLargeLens();
    emit appendText(tr("Big Lens Present: %1").arg(bigLens));
    c = dynamic_cast<ArisFrame*>(frame)->getSpeedOfSound();emit appendText(tr("Speed of Sound: %1").arg(double(c)));
    samplesPerMeter = numberOfSamples/(maxRange-minRange);
    hBeamWidth = getIndividualBeamWidth()*numberOfBeams;
}

void ArisData::readData(__attribute__((unused)) FILE *filename){
    float *centerBeam,*angles,tempFloat,*frameData;
    int i,j,k,index;
    ArisFrame *arisFrame = reinterpret_cast<ArisFrame*>(frame);

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
            arisFrame->goToFrame(0);
            arisFrame->setFirstFrame();
        }
        else arisFrame->readFrameNoTVG();
        if(!useMaximumBeam){
            centerBeam = arisFrame->centerBeam();
        }
        else{
            centerBeam = arisFrame->maximumBeam();
        }
        frameData = arisFrame->getFrame();
        angles = arisFrame->angles();
        for(j = 0; j < numberOfSamples; j++){
            //tempFloat = float(fmin(0.0,fmax(float(centerBeam[j]),-200.0)));					//Threshold between 0 and -200dB
            //amplitude[i*numberOfSamples+j] = ushort(-300*tempFloat);          //Scale to fit an unsigned short
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
        //progress.setValue(i);
    }
    readTimes();
    pingRate = float(numberOfPings)/float(endTime.toTime_t()-startTime.toTime_t());   //Adjust ping rate to reflect actual time sampled
    arisFrame->goToFrame(0);
    maxRange = arisFrame->getMaximumRange();                                                //Obtain min & max ranges and samples per meter from 1st frame
    minRange = arisFrame->getWindowStart();                                                 //rather than from file header.
    samplesPerMeter = numberOfSamples/(maxRange-minRange);
    for(i = 0; i < numberOfSamples*numberOfBeams*10; i++){
        videoBackground[i] = videoBackground[i]/(numberOfPings/10);
    }
}
