#ifndef COLORACTION_H
#define COLORACTION_H

#include <QAction>
#include <QObject>
#include <QString>
#include "imageprocessing.h"

class ColorAction : public QAction
{
    Q_OBJECT
    Color *map;
    QString name;
public:
    ColorAction(QObject *parent);
    Color *getMap();
    QString getName();
    void setMap(Color *newMap);
    void setName(QString newName);

public slots:
    void triggeredAction();

signals:
    void triggered(ColorAction *s);
};

#endif // COLORACTION_H
