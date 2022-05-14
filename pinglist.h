#ifndef PINGLIST_H
#define PINGLIST_H

/*
 *  PingList.h
 *  ArrayTest
 *
 *  Created by Carl Pfisterer on 6/24/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>


//----------------------------------------------------------
//	Structure to hold base information of a ping in a file
//----------------------------------------------------------
struct Ping {
    int fileLocation;				//Location in file of the start of the ping
    int numberOfSamples;			//Number of samples in the ping
    int pingNumber;
    Ping* next;
    Ping(){
        next = nullptr;
        fileLocation = 0;
        pingNumber = 0;
        numberOfSamples = 0;
    }
};


//------------------------------------------------------------
//	Linked list of pings to index all pings in a file
//------------------------------------------------------------
class PingList {
private:
    int size;
    int maximumNumberOfSamples;
    Ping* head;
    Ping** pingArray;
    void finalize();
protected:

public:
    PingList();
    ~PingList();
    int count();
    int numberOfSamples();
    void add(Ping* p);
    bool remove(int index);
    Ping* get(int index);
};

#endif // PINGLIST_H
