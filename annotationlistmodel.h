#ifndef ANNOTATIONLISTMODEL_H
#define ANNOTATIONLISTMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <Qt>
#include <QMap>
#include <QDebug>
#include "annotationlist.h"

class AnnotationListModel : public QAbstractTableModel
{
    Q_OBJECT
    AnnotationList *annotations;
public:
    AnnotationListModel(QObject *parent);
    int columnCount(const QModelIndex &parent) const;
    int count();
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );
    int rowCount(const QModelIndex &parent) const;
    void setData(AnnotationList *newData);

private:
    using QAbstractTableModel::setData;

public slots:
    void updateData();
};

#endif // ANNOTATIONLISTMODEL_H
