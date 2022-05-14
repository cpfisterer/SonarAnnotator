/*
 *  ImageProcessing.c
 *  Echotastic 2
 *
 *  Created by Carl Pfisterer on 7/1/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include "imageprocessing.h"

//opt_med3()
//Sorts an array of 3 values, returns the middle value
//Elements are sorted.
pixelvalue opt_med3(pixelvalue *p){
    PIX_SORT(p[0],p[1])PIX_SORT(p[1],p[2])
    PIX_SORT(p[0],p[1])
    return (p[1]);
}

//opt_med4()
//Sorts an array of 4 values, returns the average of the 2nd
// and 3rd values.  Not optimized. Elements are sorted.
pixelvalue opt_med4(pixelvalue *p){
    PIX_SORT(p[0],p[1])PIX_SORT(p[1],p[2])PIX_SORT(p[2],p[3])
    PIX_SORT(p[0],p[1])PIX_SORT(p[1],p[2])PIX_SORT(p[0],p[1])
    return pixelvalue((p[1]+p[2])/float(2.0));
}

//opt_med6()
//Sorts an array of 6 values, returns the average of the 3rd
//and 4th values.  Brute force, not optimized. Elements are sorted.
pixelvalue opt_med6(pixelvalue *p){
    PIX_SORT(p[0],p[1])PIX_SORT(p[1],p[2])PIX_SORT(p[2],p[3])
    PIX_SORT(p[3],p[4])PIX_SORT(p[4],p[5])PIX_SORT(p[0],p[1])
    PIX_SORT(p[1],p[2])PIX_SORT(p[2],p[3])PIX_SORT(p[3],p[4])
    PIX_SORT(p[0],p[1])PIX_SORT(p[1],p[2])PIX_SORT(p[2],p[3])
    PIX_SORT(p[0],p[1])PIX_SORT(p[1],p[2])PIX_SORT(p[0],p[1])
    return pixelvalue((p[2]+p[3])/float(2.0));
}

/*-------------------------------------------------------------------
 Function    :   opt_med9()
 In          :   pointer to an array of 9 pixel values
 Out         :   a pixelvalue
 Job         :   optimized search of the median of 9 pixelvalues
 Notice      :   in theory, cannot go faster without assumptions on the signal
 Forumla from:
 XILINX XCELL magazine, vol. 23 by John L. Smith

 The input array is modified in the process
 The result array is guaranteed to contain the median value
 in the middle position, but other elements are NOT sorted.
 ---------------------------------------------------------------------*/
pixelvalue opt_med9(pixelvalue * p)
{
    PIX_SORT(p[1],p[2])PIX_SORT(p[4],p[5])PIX_SORT(p[7],p[8])
    PIX_SORT(p[0],p[1])PIX_SORT(p[3],p[4])PIX_SORT(p[6],p[7])
    PIX_SORT(p[1],p[2])PIX_SORT(p[4],p[5])PIX_SORT(p[7],p[8])
    PIX_SORT(p[0],p[3])PIX_SORT(p[5],p[8])PIX_SORT(p[4],p[7])
    PIX_SORT(p[3],p[6])PIX_SORT(p[1],p[4])PIX_SORT(p[2],p[5])
    PIX_SORT(p[4],p[7])PIX_SORT(p[4],p[2])PIX_SORT(p[6],p[4])
    PIX_SORT(p[4],p[2])return(p[4]);
}

/*-------------------------------------------------------------------
 Function    :   opt_med25()
 In          :   pointer to an array of 25 pixel values
 Out         :   a pixelvalue
 Job         :   optimized search of the median of 9 pixelvalues
 Notice      :   in theory, cannot go faster without assumptions on the signal
 Forumla from:

 Code taken from Graphic Gems.
 ---------------------------------------------------------------------*/
pixelvalue opt_med25(pixelvalue * p)
{
    PIX_SORT(p[0],p[1])PIX_SORT(p[3],p[4])PIX_SORT(p[2],p[4])
    PIX_SORT(p[2],p[3])PIX_SORT(p[6],p[7])PIX_SORT(p[5],p[7])
    PIX_SORT(p[5],p[6])PIX_SORT(p[9],p[10])PIX_SORT(p[8],p[10])
    PIX_SORT(p[8],p[9])PIX_SORT(p[12],p[13])PIX_SORT(p[11],p[13])
    PIX_SORT(p[11],p[12])PIX_SORT(p[15],p[16])PIX_SORT(p[14],p[16])
    PIX_SORT(p[14],p[15])PIX_SORT(p[18],p[19])PIX_SORT(p[17],p[19])
    PIX_SORT(p[17],p[18])PIX_SORT(p[21],p[22])PIX_SORT(p[20],p[22])
    PIX_SORT(p[20],p[21])PIX_SORT(p[23],p[24])PIX_SORT(p[2],p[5])
    PIX_SORT(p[3],p[6])PIX_SORT(p[0],p[6])PIX_SORT(p[0],p[3])
    PIX_SORT(p[4],p[7])PIX_SORT(p[1],p[7])PIX_SORT(p[1],p[4])
    PIX_SORT(p[11],p[14])PIX_SORT(p[8],p[14])PIX_SORT(p[8],p[11])
    PIX_SORT(p[12],p[15])PIX_SORT(p[9],p[15])PIX_SORT(p[9],p[12])
    PIX_SORT(p[13],p[16])PIX_SORT(p[10],p[16])PIX_SORT(p[10],p[13])
    PIX_SORT(p[20],p[23])PIX_SORT(p[17],p[23])PIX_SORT(p[17],p[20])
    PIX_SORT(p[21],p[24])PIX_SORT(p[18],p[24])PIX_SORT(p[18],p[21])
    PIX_SORT(p[19],p[22])PIX_SORT(p[8],p[17])PIX_SORT(p[9],p[18])
    PIX_SORT(p[0],p[18])PIX_SORT(p[0],p[9])PIX_SORT(p[10],p[19])
    PIX_SORT(p[1],p[19])PIX_SORT(p[1],p[10])PIX_SORT(p[11],p[20])
    PIX_SORT(p[2],p[20])PIX_SORT(p[2],p[11])PIX_SORT(p[12],p[21])
    PIX_SORT(p[3],p[21])PIX_SORT(p[3],p[12])PIX_SORT(p[13],p[22])
    PIX_SORT(p[4],p[22])PIX_SORT(p[4],p[13])PIX_SORT(p[14],p[23])
    PIX_SORT(p[5],p[23])PIX_SORT(p[5],p[14])PIX_SORT(p[15],p[24])
    PIX_SORT(p[6],p[24])PIX_SORT(p[6],p[15])PIX_SORT(p[7],p[16])
    PIX_SORT(p[7],p[19])PIX_SORT(p[13],p[21])PIX_SORT(p[15],p[23])
    PIX_SORT(p[7],p[13])PIX_SORT(p[7],p[15])PIX_SORT(p[1],p[9])
    PIX_SORT(p[3],p[11])PIX_SORT(p[5],p[17])PIX_SORT(p[11],p[17])
    PIX_SORT(p[9],p[17])PIX_SORT(p[4],p[10])PIX_SORT(p[6],p[12])
    PIX_SORT(p[7],p[14])PIX_SORT(p[4],p[6])PIX_SORT(p[4],p[7])
    PIX_SORT(p[12],p[14])PIX_SORT(p[10],p[14])PIX_SORT(p[6],p[7])
    PIX_SORT(p[10],p[12])PIX_SORT(p[6],p[10])PIX_SORT(p[6],p[17])
    PIX_SORT(p[12],p[17])PIX_SORT(p[7],p[17])PIX_SORT(p[7],p[10])
    PIX_SORT(p[12],p[18])PIX_SORT(p[7],p[12])PIX_SORT(p[10],p[18])
    PIX_SORT(p[12],p[20])PIX_SORT(p[10],p[20])PIX_SORT(p[10],p[12])
    return (p[12]);
}

//----------------------------------------------------------------------------------
// convolve3x3()
// Takes as input a matrix of values and does standard convolusion
// The kernel is a 9 element vector representing a
// 3x3 matrix of values that are representing the weights of each surrounding pixel.
// w and h are the width and height of the input values.
//----------------------------------------------------------------------------------
unsigned char* convolve3x3(unsigned char* inValues, float *kernel,int w, int h){
    unsigned char* outValues = new unsigned char[(w)*(h)];
    int i,j;
    float div=0;

    for(i = 0; i < 9; i++) div=div+kernel[i];           //Sum the elements of the kernel
    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            if(i==0){                                   //Case of first row
                if(j==0){                               //Upper left corner
                    outValues[0] = uchar((kernel[4]*float(inValues[0])+kernel[5]*float(inValues[1])+kernel[7]*float(inValues[w])+
                                    kernel[8]*float(inValues[w+1]))/(kernel[4]+kernel[5]+kernel[7]+kernel[8]));
                }
                else if(j==(w-1)){                      //Upper right corner
                    outValues[i*w+j] = uchar((kernel[3]*float(inValues[w-2])+kernel[4]*float(inValues[w-1])+kernel[6]*float(inValues[2*w-2])+
                                        kernel[7]*float(inValues[2*w-1]))/(kernel[3]+kernel[4]+kernel[6]+kernel[7]));
                }
                else{
                    outValues[i*w+j] = uchar((kernel[3]*float(inValues[j-1])+kernel[4]*float(inValues[j])+kernel[5]*float(inValues[j+1])+
                                        kernel[6]*float(inValues[w+j-1])+kernel[7]*float(inValues[w+j])+kernel[8]*float(inValues[w+j+1]))/
                            (kernel[3]+kernel[4]+kernel[5]+kernel[6]+kernel[7]+kernel[8]));
                }
            }
            else if(i==(h-1)){                          //Case of last row
                if(j==0){                               //Lower left corner
                    outValues[i*w+j] = uchar((kernel[1]*float(inValues[(i-1)*w])+kernel[2]*float(inValues[(i-1)*w+1])+kernel[4]*float(inValues[i*w])+
                                        kernel[5]*float(inValues[i*w+1]))/(kernel[1]+kernel[2]+kernel[4]+kernel[5]));
                }
                else if(j==(w-1)){                      //Lower right corner
                    outValues[i*w+j] = uchar((kernel[0]*float(inValues[(i-1)*w+j-1])+kernel[1]*float(inValues[(i-1)*w+j])+
                                        kernel[3]*float(inValues[i*w+j-1])+kernel[4]*float(inValues[i*w+j]))/(kernel[0]+kernel[1]+kernel[3]+kernel[4]));
                }
                else{
                    outValues[i*w+j] = uchar((kernel[0]*float(inValues[(i-1)*w+j-1])+kernel[1]*float(inValues[(i-1)*w+j])+kernel[2]*float(inValues[(i-1)*w+j+1])+
                                        kernel[3]*float(inValues[i*w+j-1])+kernel[4]*float(inValues[i*w+j])+kernel[5]*float(inValues[i*w+j+1]))/
                                       (kernel[0]+kernel[1]+kernel[2]+kernel[3]+kernel[4]+kernel[5]));
                }
            }
            else{
                if(j==0){                               //First column
                    outValues[i*w+j] = uchar((kernel[1]*float(inValues[(i-1)*w])+kernel[2]*float(inValues[(i-1)*w+1])+kernel[4]*inValues[i*w]+
                                        kernel[5]*inValues[i*w+1]+kernel[7]*inValues[(i+1)*w]+kernel[8]*inValues[(i+1)*w+1])/
                                       (kernel[1]+kernel[2]+kernel[4]+kernel[5]+kernel[7]+kernel[8]));
                }
                else if(j==(w-1)){                      //Last column
                    outValues[i*w+j] = uchar((kernel[0]*inValues[(i-1)*w+j-1]+kernel[1]*inValues[(i-1)*w+j]+kernel[3]*inValues[i*w+j-1]+
                                        kernel[4]*inValues[i*w+j]+kernel[6]*inValues[(i+1)*w+j-1]+kernel[7]*inValues[(i+1)*w+j])/
                                       (kernel[0]+kernel[1]+kernel[3]+kernel[4]+kernel[6]+kernel[7]));
                }
                else{
                    outValues[i*w+j] = uchar(fmin(255,(kernel[0]*inValues[(i-1)*w+j-1]+kernel[1]*inValues[(i-1)*w+j]+kernel[2]*inValues[(i-1)*w+j+1]+
                                        kernel[3]*inValues[i*w+j-1]+kernel[4]*inValues[i*w+j]+kernel[5]*inValues[i*w+j+1]+
                                        kernel[6]*inValues[(i+1)*w+j-1]+kernel[7]*inValues[(i+1)*w+j]+kernel[8]*inValues[(i+1)*w+j+1])/div));
                }
            }
        }
    }
    return outValues;
}

//----------------------------------------------------------------------------------
// lowPassFilter3x3()
//  Special case of convolve 3x3 with a unity matrix passed as the kernel.
//----------------------------------------------------------------------------------
unsigned char* lowPassFilter3x3(unsigned char* inValues,int w, int h){
    float kernel[9] = {1,1,1,1,1,1,1,1,1};

    return convolve3x3(inValues, kernel, w, h);
}

//----------------------------------------------------------------------------------
// medianFilter3x3()
// Takes as input a matrix of values.
// Each returned value is the median of the adjacent
// 9 values. w and h are the width and height of the input values.
//----------------------------------------------------------------------------------
unsigned char* medianFilter3x3(unsigned char* inValues, int w, int h){
    unsigned char* outValues = new unsigned char[(w)*(h)];
    float value[3*3];
    //unsigned char value[3*3];
    int i,j;

    for(i = 0; i < h; i++){				//Loop through for each ping
        for(j = 0; j < w; j++){                         //Loop through for each sample
            if(i==0){                                   //Case of first row
                if(j==0){                               //Upper left corner
                    value[0] = inValues[0]; value[1] = inValues[1]; value[2] = inValues[w]; value[3] = inValues[w+1];
                    outValues[0] = uchar(opt_med4(value));
                }
                else if(j==(w-1)){                      //Upper right corner
                    value[0] = inValues[w-2]; value[1] = inValues[w-1]; value[2] = inValues[2*w-2]; value[3] = inValues[2*w-1];
                    outValues[i*w+j] = uchar(opt_med4(value));
                }
                else{
                    value[0] = inValues[j-1]; value[1] = inValues[j]; value[2] = inValues[j+1];
                    value[3] = inValues[w+j-1]; value[4] = inValues[w+j]; value[5] = inValues[w+j+1];
                    outValues[i*w+j] = uchar(opt_med6(value));
                }
            }
            else if(i==(h-1)){                          //Case of last row
                if(j==0){                               //Lower left corner
                    value[0] = inValues[(i-1)*w];value[1] = inValues[(i-1)*w+1];value[2] = inValues[i*w];value[3] = inValues[i*w+1];
                    outValues[i*w+j] = uchar(opt_med4(value));
                }
                else if(j==(w-1)){                      //Lower right corner
                    value[0] = inValues[(i-1)*w+j-1]; value[1] = inValues[(i-1)*w+j]; value[2] = inValues[i*w+j-1]; value[3] = inValues[i*w+j];
                    outValues[i*w+j] = uchar(opt_med4(value));
                }
                else{
                    value[0] = inValues[(i-1)*w+j-1];value[1] = inValues[(i-1)*w+j];value[2] = inValues[(i-1)*w+j+1];
                    value[3] = inValues[i*w+j-1];value[4] = inValues[i*w+j]; value[5] = inValues[i*w+j+1];
                    outValues[i*w+j] = uchar(opt_med6(value));
                }
            }
            else{
                if(j==0){                               //First column
                    value[0] = inValues[(i-1)*w];value[1] = inValues[(i-1)*w+1];value[2] = inValues[i*w];
                    value[3] = inValues[i*w+1];value[4] = inValues[(i+1)*w];value[5] = inValues[(i+1)*w+1];
                    outValues[i*w+j] = uchar(opt_med6(value));
                }
                else if(j==(w-1)){                      //Last column
                    value[0] = inValues[(i-1)*w+j-1];value[1] = inValues[(i-1)*w+j];value[2] = inValues[i*w+j-1];
                    value[3] = inValues[i*w+j];value[4] = inValues[(i+1)*w+j-1]; value[5] = inValues[(i+1)*w+j];
                    outValues[i*w+j] = uchar(opt_med6(value));
                }
                else{
                    value[0] = inValues[(i-1)*w+j-1];value[1] = inValues[(i-1)*w+j];value[2] = inValues[(i-1)*w+j+1];
                    value[3] = inValues[i*w+j-1];value[4] = inValues[i*w+j];value[5] = inValues[i*w+j+1];
                    value[6] = inValues[(i+1)*w+j-1];value[7] = inValues[(i+1)*w+j];value[8] = inValues[(i+1)*w+j+1];
                    outValues[i*w+j] = uchar(opt_med9(value));
                }
            }
        }
    }
    return outValues;
}

//----------------------------------------------------------------------------------
// medianFilter5x5()
// Takes as input a matrix of values.
// Each returned value is the median of the adjacent
// 25 values. w and h are the width and height of the input values.
//----------------------------------------------------------------------------------
unsigned char* medianFilter5x5(__attribute__((unused))unsigned char* inValues, int w, int h){
    unsigned char* outValues = new unsigned char[(w)*(h)];
    //float value[5*5];
    int i,j;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){

        }
    }

    return outValues;
}


//----------------------------------------------------------------------------------
// adjustContrast()
// Adjusts the contrast of the image.
//----------------------------------------------------------------------------------
unsigned char* adjustContrast(unsigned char* values, int w, int h, float contrast){
    unsigned char* image = new unsigned char[w*h];
    int i,j,index;
    float value;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value = float((double(values[index])-128.0)*double(contrast)+128.0);
            image[index]=uchar(fmin(255,fmax(value,0)));
        }
    }
    return image;
}


//----------------------------------------------------------------------------------
// adjustGamma()
// Adjusts the gamma of the image.
//----------------------------------------------------------------------------------
unsigned char* adjustGamma(unsigned char* values, int w, int h, float gamma){
    unsigned char* image = new unsigned char[w*h];
    int i,j,index;
    float value;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value = float(pow(values[index],gamma));
            image[index]=uchar(fmin(255,fmax(value,0)));
        }
    }
    return image;
}

//----------------------------------------------------------------------------------
// adjustContrastGamma()
// Adjusts the contrast and gamma of the image in one function.
//----------------------------------------------------------------------------------
unsigned char* adjustContrastGamma(unsigned char* values, int w, int h, float contrast, float gamma){
    unsigned char* image = new unsigned char[w*h];
    int i,j,index;
    float value;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value = float((double(values[index])-128.0)*double(contrast)+128.0);    //adjust contrast
            value = pow(value,gamma);                               //adjust gamma
            image[index] = uchar(bracket(int(value),0,255));
        }
    }
    return image;
}

//----------------------------------------------------------------------------------
//  averageImage()
//  Averages the amplitude for 3 images as a primitive super resolution.
//----------------------------------------------------------------------------------
float* averageImage(float *frame1, float *frame2, float *frame3, int w, int h){
    float *newImage = new float[w*h];
    int i,j,index;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){            
            index=min(w*h-1,i*w+j);
            newImage[index] = (frame1[index]+frame2[index]+frame3[index])/3;
        }
    }
    return newImage;
}

//----------------------------------------------------------------------------------
//  medianImage()
//  Gets the median value across each of 3 images as an alternative to averageImage().
//----------------------------------------------------------------------------------
float* medianImage(float *frame1, float *frame2, float *frame3, int w, int h){
    float *newImage = new float[w*h];
    int i,j,index;
    float value[3*3];

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value[0]=frame1[index];value[1]=frame2[index];value[2]=frame3[index];
            newImage[index] = opt_med3(value);
        }
    }
    return newImage;
}

//----------------------------------------------------------------------------------
// mapDecibelToChar()
// Maps the amplitude values in decibels to values ranging from 0-255.
// Requires a lower threshold to help with the dynamic range.  An upper threshold
// may be necessary for weak data.
//----------------------------------------------------------------------------------
unsigned char* mapDecibelToChar(float* amplitude, int w, int h, float lowerThreshold, float upperThreshold){
    unsigned char* newAmplitude = new unsigned char[w*h];
    int i,j,index;
    float dB;

    //lowerThreshold = fabs(lowerThreshold);
    float value, scaler = 255/(upperThreshold-lowerThreshold);           // Value per dB;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            dB = amplitude[index];
            value = (dB > lowerThreshold) ? (dB-lowerThreshold)*scaler:0;
            value = bracket(int(value),0,255);
            newAmplitude[index] = uchar(value);
        }
    }

    return newAmplitude;
}

unsigned char* mapDecibelToCharAdjustContrastGamma(float* amplitude, int w, int h,float lowerThreshold, float upperThreshold,float contrast,float gamma){
    unsigned char* newAmplitude = new unsigned char[w*h];
    int i,j,index;
    float dB;

    //lowerThreshold = fabs(lowerThreshold);
    float value, scaler = 255/(upperThreshold-lowerThreshold);           // Value per dB;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            dB = amplitude[index];
            value = (dB > lowerThreshold) ? (dB-lowerThreshold)*scaler:0;   //scale the floating point value between upper and lower thresholds
            value = float((double(value)-128.0)*double(contrast)+128.0);    //adjust contrast
            value = pow(value,gamma);                //adjust gamma
            value = bracket(int(value),0,255);           //bracket the value between 0 and 255 to prevent wrapping
            newAmplitude[index] = uchar(value);
        }
    }

    return newAmplitude;
}

//----------------------------------------------------------------------------------
// mapVoltageToChar()
// Maps the amplitude values in volts to values ranging from 0-255.
//----------------------------------------------------------------------------------
unsigned char* mapVoltageToChar(float* amplitude, int w, int h, float maximum,float minimum){
    unsigned char* newAmplitude = new unsigned char[w*h];
    int i, j,index;
    float value, scaler = 256/(maximum-minimum);                  // Value per volt (minimum of 0 volts, maximum 10 volts);

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value = float(pow(10,amplitude[index]/20));        //Convert to decibels
            value = (value-minimum)*scaler;
            value = bracket(int(value),0,255);
            newAmplitude[index] = uchar(value);
        }
    }

    return newAmplitude;
}

unsigned char* mapVoltageToCharAdjustContrastGamma(float* amplitude, int w, int h, float maximum, float minimum,float contrast,float gamma){
    unsigned char* newAmplitude = new unsigned char[w*h];
    int i, j,index;
    float value, scaler = 256/(maximum-minimum);                  // Value per volt (minimum of 0 volts, maximum 10 volts);

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value =float(pow(10,amplitude[index]/20));        //Convert to decibels
            value = (value-minimum)*scaler;             //Scale values
            value = float((double(value)-128.0)*double(contrast)+128.0);    //adjust contrast
            value = pow(value,gamma);                //adjust gamma
            value = bracket(int(value),0,255);           //bracket to prevent wrapping
            newAmplitude[index] = uchar(value);
        }
    }

    return newAmplitude;
}

//----------------------------------------------------------------------------------
// mapAngleToChar()
// Maps the angle values in degrees to values ranging from 0 to 255.
//----------------------------------------------------------------------------------
unsigned char* mapAngleToChar(float* angle, int w, int h, float limit){
    unsigned char* newAngle = new unsigned char[w*h];
    int i,j,index;
    float value,scaler = 256/(2*limit);	//Value per degree (plus or minus the limits in degrees)

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            index=min(w*h-1,i*w+j);
            value = (fmax(-1*limit,float(fmin(limit,angle[index])))+limit)*scaler;
            value = bracket(int(value),0,255);
            newAngle[index] = uchar(value);
        }
    }

    return newAngle;
}

//----------------------------------------------------------------------------------
// mapValuesToColor()
// Maps char values to red, blue, green values and returns a
// data buffer with the values meshed.
// First scales the decibel value
//
// * Check to see if meshed values will work with Win32 BMP *
//----------------------------------------------------------------------------------
unsigned char* mapValuesToColor(unsigned char* amplitude, Color* map, int w, int h){
    unsigned char* newColors = new unsigned char[w*h*4];	// Since mapping single value to 4 color channels each an unsigned char
    int i, j, index, index2;

    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            // Scale the amplitude
            index = min(w*h-1,i*w+j);
            index2 = min(h*w*4-1,i*w*4+j*4);
            newColors[index2] = map[min(255,amplitude[index])].red;
            newColors[index2+1] = map[min(255,amplitude[index])].green;
            newColors[index2+2] = map[min(255,amplitude[index])].blue;
            newColors[index2+3] = 255;
        }
    }

    return newColors;
}

//----------------------------------------------------------------------------------
//  removeBackground()
//  Removes the background by determining if whether each value is within +- cutoff
//  of the mean for that sample.  If it is, set the value to -300dB, otherwise, leave
//  it.  Receives a pointer to the data, a pointer to the background matrix, a rectangle
//  representing the offset and size of the data to be processed, and a rectangle
//  representing the size of the entire dataset.
//----------------------------------------------------------------------------------
float* removeBackground(float* amplitude, float* background, Rectangle r, Rectangle r2, float cutoff, bool rotate){
    float *image = new float[r.w*r.h];
    float pingsPerDiv = r2.h/10;    // The number of pings between each background row
    int i,j,temp1,index;

    for(i = 0; i < r.h; i++){
        for(j = 0; j < r.w; j++){
            //if(floor((r.x+j)/pingsPerDiv)*r2.w+r.y+i){}     // *** Not sure what this was for
            index = min(r.h*r.w-1,i*r.w+j);
            if(rotate){
                temp1 = min(10*r2.w-1,min(9,int(float(r.x+j)/pingsPerDiv))*r2.w+r.y+i);
                image[index] = (amplitude[index] < (background[temp1] + cutoff)) ? -300.0:amplitude[index];
                //image[(int)i*r.w+j] = (fabs(amplitude[(int)i*r.w+j] - background[(int)floor((r.x+j)/pingsPerDiv)*r2.w+r.y+i]) < cutoff) ? -300.0:amplitude[(int)i*r.w+j];
            }
            else{
                temp1 = min(10*r2.w-1,min(9,int(float(r.y+i)/pingsPerDiv))*r2.w+r.x+j);
                image[index] = (amplitude[index] < (background[temp1] + cutoff)) ? -300.0:amplitude[index];
                //image[(int)i*r.w+j] = (fabs(amplitude[(int)i*r.w+j] - background[(int)floor((r.y+i)/pingsPerDiv)*r2.w+r.x+j]) < cutoff) ? -300.0:amplitude[(int)i*r.w+j];
            }
        }
    }

    return image;
}

float* removeBackground(float* amplitude, float* background, float* se, Rectangle r, Rectangle r2, float cutoff, bool rotate){
    float *image = new float[r.w*r.h];
    float pingsPerDiv = r2.h/10;    // The number of pings between each background row
    int i,j,index;
    int temp1;

    for(i = 0; i < r.h; i++){
        for(j = 0; j < r.w; j++){
            index = min(r.h*r.w-1,i*r.w+j);
            if(rotate){
                temp1 = min(10*r2.w-1,min(9,int(float(r.x+j)/pingsPerDiv))*r2.w+r.y+i);
                image[index] = (amplitude[index] < (background[temp1] + cutoff*se[temp1]))
                                      ? -300:amplitude[index];
                //image[(int)i*r.w+j] = (fabs(amplitude[(int)i*r.w+j] - background[(int)floor((r.x+j)/pingsPerDiv)*r2.w+r.y+i]) < cutoff*se[(int)floor((r.x+j)/pingsPerDiv)*r2.w+r.y+i])
                //                      ? -300:amplitude[i*r.w+j];
            }
            else{
                temp1 = min(10*r2.w-1,min(9,int(float(r.y+i)/pingsPerDiv))*r2.w+r.x+j);
                image[index] = (amplitude[index] < (background[temp1] + cutoff*se[temp1]))
                                      ? -300:amplitude[index];
                //image[(int)i*r.w+j] = (fabs(amplitude[(int)i*r.w+j] - background[(int)floor((r.y+i)/pingsPerDiv)*r2.w+r.x+j]) < cutoff*se[(int)floor((r.y+i)/pingsPerDiv)*r2.w+r.x+j])
                //                      ? -300:amplitude[i*r.w+j];
            }
        }
    }

    return image;
}

float* removeBackground(float* amplitude, float* background, int frameNumber, int totalFrames,int width, int height, float cutoff){
    float *image = new float[width*height];
    float framesPerDiv = totalFrames/10;
    int i,j,index1,index2;
    int backgroundFrameNumber = int(fmin(9,floor(frameNumber/framesPerDiv)));//fmin(9,frameNumber/framesPerDiv);

    index2 = backgroundFrameNumber*width*height;
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            index1 = (i*width+j);
            //if(fabs(amplitude[index1]-background[index2]) < cutoff){  //Allows values greater or smaller than average
            if(amplitude[index1] < ((background[index2+index1] + cutoff))){    //Only allows values greater than average
                image[index1] = -200.0;
            }
            else{
                image[index1] = amplitude[index1];
            }
        }
    }

    //printf("frameNumber = %i  totalFrame=%i  width=%i  height=%i backgroundStart%i\n",frameNumber,totalFrames,width,height,backgroundFrameNumber);
    return image;
}


//----------------------------------------------------------------------------------
//  subtractBackground()
//  Subtracts the background value from the image instead of removing background.
//----------------------------------------------------------------------------------
float* subtractBackground(float *amplitude, float* background, Rectangle r, Rectangle r2,__attribute__((unused)) bool rotate){
    float *image = new float[r.w*r.h];
    float pingsPerDiv = r2.h/10;    // The number of pings between each background row
    int i,j,index;

    for(i = 0; i < r.h; i++){
        for(j = 0; j < r.w; j++){
            index = min(r.h*r.h-1,i*r.w+j);
            image[index] = (amplitude[index]+300 - (background[int(floor((r.y+i)/pingsPerDiv))*r2.w+r.x+j]+300))-300;
        }
    }

    return image;
}

//----------------------------------------------------------------------------------
//  subtractBackground()
//  Subtracts the background value from the image instead of removing background.
//----------------------------------------------------------------------------------
float* subtractBackground(float *amplitude, float* background,int frameNumber, int totalFrames,int width, int height){
    float *image = new float[width*height];
    float framesPerDiv = float(totalFrames)/10.0;
    int i,j,index1,index2;
    int backgroundFrameNumber = int(fmin(9.0,floor(float(frameNumber)/framesPerDiv)));//fmin(9,frameNumber/framesPerDiv);

    index2 = backgroundFrameNumber*width*height;
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            index1 = (i*width+j);
            image[index1] = max(0.0,float(amplitude[index1] - (background[index2+index1])));
        }
    }

    //printf("frameNumber = %i  totalFrame=%i  width=%i  height=%i backgroundStart%i\n",frameNumber,totalFrames,width,height,backgroundFrameNumber);
    return image;
}

//----------------------------------------------------------------------------------
//  duplicateImage()
//  Returns a duplicate of the image passed
//----------------------------------------------------------------------------------
float* duplicateImage(float* data, int width, int height){
    float *image = new float[height*width];
    int i;

    for(i = 0; i < height*width; i++){
        image[i] = data[i];
    }

    return image;
}

//----------------------------------------------------------
//  Below are member functions for class ImageFilter
//----------------------------------------------------------
float* ImageFilter::process(float* data, Rectangle r){
    float* values = new float[r.w*r.h];
    int i;

    qDebug() << "Call default process method of base class";

    for(i = 0; i < (r.w*r.h); i++){
        values[i] = data[i];
    }

    return values;          //Returns original image
}

ImageFilter::~ImageFilter(){}

//----------------------------------------------------------
//  Below are member functions for class AverageImage
//----------------------------------------------------------
float* AverageImage::process(float *data, float *data2, float *data3, Rectangle r){
    return averageImage(data, data2, data3, r.w, r.h);
}

//----------------------------------------------------------
//  Below are member functions for class ContrastGammaFilter
//----------------------------------------------------------
float* ContrastGammaFilter::process(float* data, Rectangle r){
    return reinterpret_cast<float*>(adjustContrastGamma(reinterpret_cast<uchar*>(data), r.w, r.h,contrast,gamma));
}

void ContrastGammaFilter::setContrast(float value){
    contrast = value;
}

void ContrastGammaFilter::setGamma(float value){
    gamma = value;
}

//----------------------------------------------------------
//  Below are member functions for class ConvolveFilter
//----------------------------------------------------------
ConvolveFilter::ConvolveFilter(){
    kernel = new float[9];
}

ConvolveFilter::~ConvolveFilter(){
    if(kernel) delete [] kernel;
}

float* ConvolveFilter::process(float* data, Rectangle r){
    return reinterpret_cast<float*>(convolve3x3(reinterpret_cast<uchar*>(data),kernel, r.w,r.h));
}

void ConvolveFilter::setKernel(float *values){
    if(kernel) delete [] kernel;
    kernel = values;
}

//----------------------------------------------------------
//  Below are member functions for class CorrectTVGFilter
//----------------------------------------------------------
float* CorrectTVGFilter::process(__attribute__((unused)) float* data, float start, float mps){
    int i;
    float tvgvalues[512];
    float *image = new float[512*96];

    for(i = 0; i < 512; i++){
        tvgvalues[i] = 40*log10(i*mps+start);
    }

    return image;
}

//----------------------------------------------------------
//  Below are member functions for class LowPassFilter
//----------------------------------------------------------
LowPassFilter::LowPassFilter(){
    int i;

    kernel = new float[9];
    for(i = 0; i < 9; i++){
        kernel[i] = 1/9;
    }
}

LowPassFilter::~LowPassFilter(){
    if(kernel) delete [] kernel;
}

float* LowPassFilter::process(float* data, Rectangle r){
    return reinterpret_cast<float*>(lowPassFilter3x3(reinterpret_cast<uchar*>(data), r.w, r.h));
}

//----------------------------------------------------------
//  Below are member functions for class MapAngleToChar
//----------------------------------------------------------
float* MapAngleToChar::process(float *data, Rectangle r){
    return reinterpret_cast<float*>(mapAngleToChar(data,r.w,r.h,limit));
}

void MapAngleToChar::setLimit(float value){
    limit = value;
}

//----------------------------------------------------------
//  Below are member functions for class MapDecibelToChar
//----------------------------------------------------------
float* MapDecibelToChar::process(float* data, Rectangle r){
    //return (float*)mapDecibelToChar(data, r.w, r.h, lower, upper);
    return reinterpret_cast<float*>(mapDecibelToCharAdjustContrastGamma(data,r.w,r.h,lower,upper,contrast,gamma));
}

void MapDecibelToChar::setContrastGamma(float value1,float value2){
    contrast = value1;
    gamma = value2;
}

void MapDecibelToChar::setLower(float value){
    lower = value;
}

void MapDecibelToChar::setUpper(float value){
    upper = value;
}

//----------------------------------------------------------
//  Below are member functions for class MapVoltageToChar
//----------------------------------------------------------
float* MapVoltageToChar::process(float* data, Rectangle r){
    //return (float*)mapVoltageToChar(data, r.w, r.h, upper, lower);
    return reinterpret_cast<float*>(mapVoltageToCharAdjustContrastGamma(data, r.w, r.h, upper, lower, contrast, gamma));
}

void MapVoltageToChar::setContrastGamma(float value1, float value2){
    contrast = value1;
    gamma = value2;
}

void MapVoltageToChar::setLower(float value){
    lower = value;
}

void MapVoltageToChar::setUpper(float value){
    upper = value;
}

//----------------------------------------------------------
//  Below are member functions for class LowPassFilter
//----------------------------------------------------------
float* MedianFilter::process(float* data, Rectangle r){
    return reinterpret_cast<float*>(medianFilter3x3(reinterpret_cast<uchar*>(data), r.w, r.h));
}

//----------------------------------------------------------
//  Below are member functions for class RemoveBackground
//----------------------------------------------------------
float* RemoveBackground::process(float* data, float* background, float* sdDev, Rectangle r, Rectangle r2){
    return reinterpret_cast<float*>(removeBackground(data,background,sdDev,r,r2,cutoff,rotate));
}

float* RemoveBackground::process(float *data, float *background, int frameNumber, int totalFrames, Rectangle r){
    return reinterpret_cast<float*>(removeBackground(data,background,frameNumber,totalFrames,r.w,r.h,cutoff));
}

void RemoveBackground::setCutoff(float value){
    cutoff = value;
}

void RemoveBackground::setRotate(bool value){
    rotate = value;
}


