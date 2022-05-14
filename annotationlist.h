#ifndef ANNOTATIONLIST_H
#define ANNOTATIONLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>


//----------------------------------------------------------
//	Structure to hold mark information
//----------------------------------------------------------
struct Annotation {
    int frame;
    float x;
    float y;
    float width;
    float height;
    char category[26];
    Annotation* next;
    Annotation(){
        next = nullptr;
        frame = 0;
        x = 0;
        y = 0;
        width = 0;
        height = 0;
        strcpy(category, "Fish");
    }
};

//------------------------------------------------------------
//	Linked list of annotations
//------------------------------------------------------------
class AnnotationList
{
private:
    void finalize();
    Annotation* head;
    Annotation** annotationArray;
    void printOrder(int col);
    int size;
    void swap(int i,int j);
    float sortValue(int i,int col);
    bool keepSorted,ascend;
    int sortCol;
public:
    AnnotationList();
    ~AnnotationList();
    void add(Annotation* a);
    void clear();
    int count();
    Annotation* get(int index);
    bool remove(int index);
    void sort(int col,bool ascending);
};

#endif // ANNOTATIONLIST_H
