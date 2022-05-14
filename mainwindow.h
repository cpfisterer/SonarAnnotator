#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QList>
#include <QTimer>
#include <QDebug>
#include <QSettings>

#include "sonardata.h"
#include "didsondata.h"
#include "didsonframe.h"
#include "arisdata.h"
#include "arisframe.h"
#include "vwidget.h"
#include "coloraction.h"
#include "annotationlistmodel.h"
#include "annotationlist.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
    void deleteSelection();
    void exportCSV(AnnotationList *list);
    void exportFrames(AnnotationList *list);
    void exportJSON(AnnotationList *list);
    void exportConfiguration();
    QString importConfiguration();
    void importCSV();
    void loadSettings();
    void saveSettings();
    std::vector<int> uniqueFrames(AnnotationList *list);

private:
    Ui::MainWindow *ui;
    SonarData *sonarData;
    QStringList fileList;
    int fileIndex;
    QString fileName,currentDirectory;
    Color amberColorMap[256],BioSonicsColorMap[256],blueColorMap[256],blueGreenRedColorMap[256],bwColorMap[256],brownColorMap[256],brownWhiteColorMap[256];
    Color greenColorMap[256],HTIColorMap[256],SimradColorMap[256],wbColorMap[256],userMap[256];
    QList<ColorAction*> colors;
    bool playing;
    QTimer *timer;
    AnnotationListModel *dataModel;

    void addAnnotation(int categoryNumber);
    void initializeColorMaps();
    void openFile(char *name,QString type);
    void openFileAtIndex(int index);
    void setupActions();

private slots:
    void about();
    void changeAlpha(double value);
    void changeBackgroundThreshold(double value);
    void changeColorMap(int index);
    void changeLowerThreshold(int value);
    void changeTVG(double value);
    void changeUpperThreshold(int value);
    void goToSelection(__attribute__((unused)) const QItemSelection &selected,__attribute__((unused)) const QItemSelection &deselected);
    void toggleBackgroundRemoval(bool value);
    void toggleAcrossFrameFilters(bool value);
    void nextFrame();
    void open();
    void play();
    void previousFrame();
    void receiveKeyPress(QKeyEvent *event);
    void exportAnnotations();

signals:
    void fileLocation(int frameNumber);
};
#endif // MAINWINDOW_H
