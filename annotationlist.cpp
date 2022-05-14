#include "annotationlist.h"

AnnotationList::AnnotationList()
{
    size = 0;
    head = nullptr;
    annotationArray = nullptr;
    keepSorted = false;
    ascend = true;
    sortCol = 5;
}

AnnotationList::~AnnotationList(){
    clear();
}

void AnnotationList::add(Annotation* a){
    a->next = head;
    head = a;
    size++;
    if(annotationArray){
        delete [] annotationArray;
        annotationArray = nullptr;
    }
    if(keepSorted){
        this->sort(sortCol,ascend);
    }
}

void AnnotationList::clear(){
    Annotation* a = head;
    Annotation* next;

    size = 0;
    while(a){
        next = a->next;
        delete a;
        a = next;
    }
    if(annotationArray){
        delete [] annotationArray;
        annotationArray = nullptr;
    }
    head = nullptr;
}

int AnnotationList::count(){
    return size;
}

void AnnotationList::finalize(){
    Annotation * current = head;

    if(annotationArray) delete [] annotationArray;
    annotationArray = new Annotation*[size];
    for(int i = size-1;i >= 0 ;i--){
        annotationArray[i] = current;
        current = current->next;
    }
}

Annotation* AnnotationList::get(int index){
    if(!annotationArray)finalize();
    if(index>=0 && index<size) return annotationArray[index];
    else return nullptr;
}

void AnnotationList::printOrder(int col){
    for(int i = 0; i < count(); i++){
        qDebug() << i << col << sortValue(i,col);
    }
}

bool AnnotationList::remove(int index){
    if(get(index) == nullptr || index>(size-1)){
        return false;
    }
    else{
        Annotation* a = get(index);
        if(index==size-1){
            head=a->next;
        }
        else{
            get(index+1)->next = get(index-1);
        }
        delete a;
        size--;
        if(annotationArray){
            delete [] annotationArray;
            annotationArray = nullptr;
        }
        if(keepSorted){
            this->sort(sortCol,ascend);
        }
        return true;
    }
}

void AnnotationList::sort(int col,bool ascending){
    int pass,index;

    keepSorted = true;
    sortCol=col;
    ascend = ascending;
    for(pass = 0;pass < (count()-1);pass++){
        for(index = count()-2;index >= 0; index--){
            if(ascending){
                //qDebug() << "Pass:" << pass<< "Value at index" << index <<sortValue(index,col) << " Value at index"<< index+1 << sortValue(index+1,col);
                if(col<5){
                    if(sortValue(index,col) > sortValue(index+1,col)){
                        swap(index,index+1);
                    }
                }
                else{
                    if(strcmp(get(index)->category,get(index+1)->category) > 0){
                        swap(index,index+1);
                    }
                }
            }
            else{
                if(col<5){
                    if(sortValue(index,col) < sortValue(index+1,col)){
                        swap(index,index+1);
                    }
                }
                else{
                    if(strcmp(get(index)->category,get(index+1)->category) < 0){
                        swap(index,index+1);
                    }
                }
            }
        }
        //this->printOrder(col);
    }
}

float AnnotationList::sortValue(int i, int col){
    switch(col){
        case 0: return float(get(i)->frame);
        case 1: return float(get(i)->x);
        case 2: return float(get(i)->y);
        case 3: return float(get(i)->width);
        case 4: return float(get(i)->height);
        default: return 0;
    }
}

void AnnotationList::swap(int i, int j){
    Annotation *a1,*a2,*a3;

    if(j<i){
        int k = i;
        i = j;
        j = k;
    }
    a1 = get(i);
    a2 = get(j);
    a3 = a1->next;
    if((j-i)>1){
        a1->next = a2->next;
        get(i+1)->next = a2;
    }
    else{
        a1->next = a2;
    }
    a2->next = a3;
    if(j<(size-1)){
        get(j+1)->next = a1;
    }
    else{
        head=a1;
    }
    finalize();
}
