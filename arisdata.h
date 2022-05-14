#ifndef ARISDATA_H
#define ARISDATA_H

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QProgressDialog>
#include "arisframe.h"
#include "didsondata.h"

class ArisData: public DIDSONData{
    Q_OBJECT

public:
    ArisData();

protected:
    int hResolution;
    void readConfiguration(FILE *filename);
    void readConfigurationV5(FILE *filename);
    void readData(FILE *filename);
};

#endif // ARISDATA_H
