#include "coloraction.h"

ColorAction::ColorAction(QObject * parent):
        QAction(parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(triggeredAction()));
    setCheckable(true);
    map = nullptr;
    this->setName(tr("Color"));
}

Color* ColorAction::getMap(){
    return map;
}

QString ColorAction::getName(){
    return name;
}

void ColorAction::setMap(Color *newMap){
    map = newMap;
}

void ColorAction::setName(QString newName){
    name = newName;
    this->setText(name);
}

void ColorAction::triggeredAction(){
    emit triggered(this);
}
