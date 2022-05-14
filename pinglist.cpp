/*
 *  PingList.cpp
 *  ArrayTest
 *
 *  Created by Carl Pfisterer on 6/24/09.
 *  Copyright 2009 Alaska Deptartment of Fish and Game. All rights reserved.
 *
 */

#include "pinglist.h"


//----------------------------------------------------------------------------------
//	PingList is a class that creates a linked list of values of type Ping.
//	The pointer to a Ping object is passed to the add() function and it is
//	added to the list.  Once the Ping has been added, the memory will be
//	managed by the PingList so it should not be deleted elsewhere in code.
//	Likewise, a reference to the Ping should not be maintained since it
//	could be deleted by the Ping list without the creating code knowing.
//	The PingList maintains an array of pointers to the Ping objects to
//	allow for fast retrieval of items with large lists.
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------
//	PingList()
//	Constructor for the PingList class.  Sets all intitial values to
//	zero or NULL
//----------------------------------------------------------------------
PingList::PingList(){
    size = 0;
    head = nullptr;
    maximumNumberOfSamples = 0;
    pingArray = nullptr;
}

//----------------------------------------------------------------------
//	~PingList()
//	Deconstructor for PingList class.  Deletes all Ping items in the
//	list and then deletes the ping array.
//----------------------------------------------------------------------
PingList::~PingList(){
    Ping* p = head;
    Ping* next;

    while(p){
        next = p->next;
        delete p;
        p = next;
    }
    if(pingArray){
        delete [] pingArray;
        pingArray = nullptr;
    }
}

//----------------------------------------------------------------------
//	count()
//	Returns the size of the linked list
//----------------------------------------------------------------------
int PingList::count(){
    return size;
}

//----------------------------------------------------------------------
//	numberOfSamples()
//	Returns the maximum number of digital samples in the file
//----------------------------------------------------------------------
int PingList::numberOfSamples(){
    return maximumNumberOfSamples;
}

//----------------------------------------------------------------------
//	add(Ping* p)
//	Adds a Ping to the end of the linked list then deletes the array and
//	sets to NULL
//----------------------------------------------------------------------
void PingList::add(Ping* p){
    p->next = head;
    head = p;
    if(p->numberOfSamples>maximumNumberOfSamples){
        maximumNumberOfSamples = p->numberOfSamples;
    }
    size++;
    if(pingArray){
        delete [] pingArray;
        pingArray = nullptr;
    }
}

//----------------------------------------------------------------------
//	remove(int index)
//	Removes item at position index in the linked list then
//	deletes the array and sets to NULL
//----------------------------------------------------------------------
bool PingList::remove(int index){
    if(get(index) == nullptr || index>(size-1)){
        return false;
    }
    else{
        Ping* p = get(index);
        if(index==size-1){
            head=p->next;
        }
        else{
            get(index+1)->next = get(index-1);
        }
        printf("Ping to be deleted %i  Location %i at index %i\n\n",p->pingNumber,p->fileLocation,index);
        delete p;
        size--;
        if(pingArray){
            delete [] pingArray;
            pingArray = nullptr;
        }
        return true;
    }
}

//----------------------------------------------------------------------
//	get(int index)
//	Retrieves item at the position index in the array or linked list
//----------------------------------------------------------------------
Ping* PingList::get(int index){
    /*Ping* current = head;

        for(int i = count()-1;i > index && current != NULL;i--){
                current = current->next;
        }

        return current;*/
    if(!pingArray)finalize();
    if(index>=0 && index<size) return pingArray[index];
    else return nullptr;
}


//----------------------------------------------------------------------
//	finalize()
//	This function creates an array from the linked list to make
//	item retrieval faster for large arrays
//----------------------------------------------------------------------
void PingList::finalize(){
    Ping * current = head;

    if(pingArray) delete [] pingArray;
    pingArray = new Ping*[size];
    for(int i = size-1;i>=0;i--){
        pingArray[i] = current;
        current = current->next;
    }
}
