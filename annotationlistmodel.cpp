#include "annotationlistmodel.h"

AnnotationListModel::AnnotationListModel(QObject *parent):
    QAbstractTableModel(parent)
{
    annotations = nullptr;
}

//---------------------------------------------------------------
//  columnCount()
//  Returns the number of columns in the marks list.  Currently
//  hard coded at 11.  Would probably be better to query the mark
//  list.
//---------------------------------------------------------------
int AnnotationListModel::columnCount(__attribute__((unused)) const QModelIndex &parent) const{
    return 6;
}

//---------------------------------------------------------------
// count()
// Returns the number of marks.
//---------------------------------------------------------------
int AnnotationListModel::count(){
    if(annotations) return annotations->count();
    else return 0;
}

//---------------------------------------------------------------
//  data()
//  Returns the value in the mark list at the coordinates given
//  by index.
//---------------------------------------------------------------
QVariant AnnotationListModel::data(const QModelIndex &index, int role) const{
    Annotation *annotation;

    if(!index.isValid()) return QVariant(0);

    if(role == Qt::DisplayRole){
        if(annotations && (annotations->count()>0)){
            if((index.row() >= 0) && (index.row() < (annotations->count()))){
                annotation = annotations->get(index.row());
                switch(index.column()){
                case(0):
                    return QVariant(annotation->frame);
                case(1):
                    return QVariant(round(annotation->x*100)/100);
                case(2):
                    return QVariant(round(annotation->y*100)/100);
                case(3):
                    return QVariant(round(annotation->width*100)/100);
                case(4):
                    return QVariant(round(annotation->height*100)/100);
                case(5):
                    return QVariant(annotation->category);
                default:
                    return QVariant(0);
                }
            }
        }
    }
    return QVariant();
}

//---------------------------------------------------------------
//  headerData()
//  Returns the header descriptions.
//---------------------------------------------------------------
QVariant AnnotationListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal){
        switch(section){
            case(0):
                return QString("Frame #");
            case(1):
                return QString("   x   ");
            case(2):
                return QString("   y   ");
            case(3):
                return QString("width");
            case(4):
                return QString("height");
            case(5):
                return QString("Category");
            default:
                return QString("Empty");
        }
    }
    else
        return QString("%1").arg(section);
}

//---------------------------------------------------------------
//  rowCount()
//  Returns the number of rows in the annotations list
//---------------------------------------------------------------
int AnnotationListModel::rowCount(__attribute__((unused)) const QModelIndex &parent) const{
    if(annotations)
        return annotations->count();
    else
        return 0;
}

//---------------------------------------------------------------
//  setData()
//  Sets the annotations list to the new data.
//---------------------------------------------------------------
void AnnotationListModel::setData(AnnotationList *newData){
    annotations = newData;
}

//---------------------------------------------------------------
//  sort()
//  Overrides the virtual function to sort the data by the specified
//  column.
//---------------------------------------------------------------
void AnnotationListModel::sort(int column, Qt::SortOrder order){
    bool ascending = false;
    //qDebug() << "Sorting data by column:" << column;

    if(order==Qt::AscendingOrder){
        ascending = true;
    }
    if(annotations){
        annotations->sort(column,ascending);
    }
    updateData();
}

//Trying to cause the dataview to update but not working so great.
void AnnotationListModel::updateData(){
    //QModelIndex top,bottom;

    //emit dataChanged(createIndex(0,0),createIndex(marks->count()-1,6));
    emit layoutChanged();
}
