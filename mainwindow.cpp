#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    sonarData = nullptr;
    dataModel = new AnnotationListModel(this);
    this->setWindowTitle("Sonar Annotator");
    timer = new QTimer(this);
    playing = false;
    ui->setupUi(this);
    this->initializeColorMaps();
    ui->video->setColorMap(blueColorMap);
    ui->echogram->setColorMap(blueColorMap);
    ui->echogram->setScalers(1.0,1.0);
    dataModel->setData(ui->video->getAnnotationList());
    ui->annotationsTable->setModel(dataModel);
    ui->annotationsTable->resizeColumnsToContents();
    this->loadSettings();

    this->setupActions();
}

MainWindow::~MainWindow()
{
    this->saveSettings();
    delete ui;
}

void MainWindow::addAnnotation(int category){
    QString c;
    switch(category){
    case 1:
        c = ui->category1->text();
        break;
    case 2:
        c = ui->category2->text();
        break;
    case 3:
        c = ui->category3->text();
        break;
    case 4:
        c = ui->category4->text();
        break;
    case 5:
        c = ui->category5->text();
        break;
    default:
        c = "Fish";
        break;
    }

    ui->video->addAnnotation(c);
}

void MainWindow::about(){
    QMessageBox::about(this,tr("About Sonar Annotator"),tr("Sonar Annotator\n\nBy\nCarl T. Pfisterer\nAlaska Dept of Fish & Game\nDivision of Commercial Fisheries\nRegion 3"));
    return;
}

void MainWindow::changeAlpha(double value){
    sonarData->setAlpha(float(value));
    ui->video->goToFrame(ui->video->getFrameNumber()-1);
    ui->echogram->update();
}

void MainWindow::changeBackgroundThreshold(double value){
    ui->video->setBackgroundCutoff(float(value));
}

void MainWindow::changeColorMap(int index){
    switch(index){
    case 0:
        ui->video->setColorMap(amberColorMap);
        ui->echogram->setColorMap(amberColorMap);
        break;
    case 1:
        ui->video->setColorMap(blueColorMap);
        ui->echogram->setColorMap(blueColorMap);
        break;
    case 2:
        ui->video->setColorMap(bwColorMap);
        ui->echogram->setColorMap(bwColorMap);
        break;
    case 3:
        ui->video->setColorMap(brownWhiteColorMap);
        ui->echogram->setColorMap(brownWhiteColorMap);
        break;
    }
}

void MainWindow::changeLowerThreshold(int value){
    ui->video->setLowerThreshold(value);
    ui->echogram->setLowerThreshold(float(value));
}

void MainWindow::changeTVG(double value){
    sonarData->setTVG(float(value));
    ui->video->goToFrame(ui->video->getFrameNumber()-1);
    ui->echogram->update();
}

void MainWindow::changeUpperThreshold(int value){
    ui->video->setUpperThreshold(value);
    ui->echogram->setUpperThreshold(float(value));
}

void MainWindow::deleteSelection(){
    QModelIndexList selection = ui->annotationsTable->selectionModel()->selectedRows();
    if(selection.count()>0){
        ui->video->deleteAnnotation(selection.at(0).row());
        ui->annotationsTable->selectionModel()->clear();
    }
}

void MainWindow::exportAnnotations(){
    AnnotationList *list = ui->video->getAnnotationList();

    if(!sonarData){
        QMessageBox::information(this,tr("No Open File"),tr("No file open, cannot save"),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton,QMessageBox::NoButton);
        return;
    }
    exportCSV(list);
    exportFrames(list);
    exportJSON(list);
    exportConfiguration();
}

void MainWindow::exportConfiguration(){
    QString configString = QString();
    QFile file(fileName+".cfg");
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information( this, tr("Error saving configuration"), tr("Error opening configuration file, no configuration saved"),
                                  QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }
    QTextStream out(&file);
    out << ui->ConfigBox->toPlainText().toLocal8Bit();

    file.close();
}

void MainWindow::exportCSV(AnnotationList *list){
    FILE* file;
    char *name,*name2;
    char extention[5] = ".csv";
    QByteArray tempByteArray;

    tempByteArray = fileName.toLocal8Bit();
    name = tempByteArray.data();
    name2 = reinterpret_cast<char*>(calloc((strlen(name)+4+3),sizeof(char)));
    strcat(name2,name);
    strcat(name2,extention);

    if((file = fopen(name2,"w"))==nullptr){
        QMessageBox::information( this, tr("Error saving csv file"), tr("Error opening csv file, no csv file saved"),
                                  QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }
    fprintf(file,"Mark Number,Frame,X,Y,Width,Height,Category\n");
    for(int i = 0; i < list->count(); i++){
        Annotation *a = list->get(i);
        float w = a->width;
        float h = a->height;
        float x = a->x;
        float y = a->y;
        fprintf(file,"%i,%i,%6.2f,%6.2f,%6.2f,%6.2f,%s\n",i,a->frame,double(x),double(y),double(w),double(h),a->category);
    }
    fclose(file);
    free(name2);
}

void MainWindow::exportFrames(AnnotationList *list){
    std::vector<int> frames = uniqueFrames(list);
    for(int i = 0; i < int(frames.size()); i++){
        ui->video->goToFrame(frames.at(i));
        QImage *image = ui->video->getImage();
        QString file = QString(fileName+tr("_%1.jpg").arg(ui->video->getFrameNumber()));
        image->save(file,"JPG",100);
    }
}

void MainWindow::exportJSON(AnnotationList *list){
    FILE* file;
    char *name,*name2;
    char extention[6] = ".json";
    QByteArray tempByteArray;
    std::vector<int> frames = uniqueFrames(list);
    bool firstFrame=true;

    tempByteArray = fileName.toLocal8Bit();
    name = tempByteArray.data();
    name2 = reinterpret_cast<char*>(calloc((strlen(name)+4+4),sizeof(char)));
    strcat(name2,name);
    strcat(name2,extention);

    if((file = fopen(name2,"w"))==nullptr){
        QMessageBox::information( this, tr("Error saving JSON file"), tr("Error opening JSON file, no annotations saved"),
                                  QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }
    fprintf(file,"[");
    for(int i = 0; i < int(frames.size()); i++){
        bool first = true;
        if(!firstFrame){
            fprintf(file,",\n\t");
        }
        else{
            fprintf(file,"\n\t");
        }
        QString shortName = QFileInfo(fileName).fileName();
        tempByteArray = QString(shortName+"_"+tr("%1").arg(int(frames.at(i)))+".jpg").toLocal8Bit();  /////Need to use tr("").arg()
        char *name3 = tempByteArray.data();
        fprintf(file,"{\n\t\t\"image\": \"%s\",\n\t\t\"annotations\": [\n",name3);
        for(int j = 0; j < list->count(); j++){
            if(list->get(j)->frame==frames.at(i)){
                if(!first){
                    fprintf(file,",\n");
                }
                else{
                    first = false;
                }
                Annotation *a = list->get(j);
                float w = a->width*float(ui->video->rect().width());
                float h = a->height*float(ui->video->rect().height());
                float x = a->x*float(ui->video->rect().width()) + w/2;
                float y = a->y*float(ui->video->rect().height()) + h/2;
                fprintf(file,"\t\t\t{\n\t\t\t\t\"label\": \"%s\",\n\t\t\t\t\"coordinates\": {\n\t\t\t\t\t\"x\": %i\n\t\t\t\t\t\"y\": %i\n\t\t\t\t\t\"width\": %i\n\t\t\t\t\t\"height\": %i\n\t\t\t\t}\n\t\t\t}",a->category,int(x),int(y),int(w),int(h));
            }
        }
        fprintf(file,"\n\t\t]\n\t}");
        firstFrame=false;
    }
    fprintf(file,"\n]");
    fclose(file);
    free(name2);
}

void MainWindow::goToSelection(__attribute__((unused)) const QItemSelection &selected,__attribute__((unused)) const QItemSelection &deselected){

    QModelIndexList selection = ui->annotationsTable->selectionModel()->selectedRows();
    if(selection.count()>0){
        Annotation *a = ui->video->getAnnotationList()->get(selection.at(0).row());
        int frame = a->frame;
        ui->video->goToFrame(frame);
        ui->video->setSelectedAnnotation(a);
        emit fileLocation(ui->video->getFrameNumber());
    }
}

QString MainWindow::importConfiguration(){
    QString configString = QString();
    QFile file(fileName+".cfg");
    if(!file.open(QIODevice::ReadOnly)) {
        return configString;    //Returns empty string
    }
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        configString.append(line+"\n");
    }
    file.close();
    return configString;
}

void MainWindow::importCSV(){
    FILE* file;
    AnnotationList *list = ui->video->getAnnotationList();
    Annotation *a;
    char *name,*name2;
    char category[26],buffer[255],extention[5] = ".csv";
    int frame,number;
    float x,y,w,h;
    int i;
    QByteArray tempByteArray;

    tempByteArray = fileName.toLocal8Bit();
    name = tempByteArray.data();
    name2 = reinterpret_cast<char*>(calloc((strlen(name)+4+3),sizeof(char)));
    strcat(name2,name);
    strcat(name2,extention);
    if((file = fopen(name2,"r"))==nullptr){
        return;                             //No existing marks file
    }
    fgets(buffer,255,file);                 //Get first line which is the header
    while(fgets(buffer,254,file)!=nullptr)
    {
        a = new Annotation();
        sscanf(buffer,"%i,%i,%f,%f,%f,%f,%s",&number,&frame,&x,&y,&w,&h,category);
        a->frame = frame;
        a->x = x;
        a->y = y;
        a->width = w;
        a->height = h;
        strcpy(a->category,category);
        list->add(a);
    }
    free(name2);
    fclose(file);
    ui->video->broadcastAnnotationChange();
}

void MainWindow::initializeColorMaps(){
    int i;

    //  Initialize the simple 3 color maps
    for(i = 0; i < 128; i++){
        blueColorMap[i].red = uchar(i);blueColorMap[i].green = uchar(i);blueColorMap[i].blue = uchar(i*2);
        amberColorMap[i].red = uchar(i*2);amberColorMap[i].green = uchar(i/1.75);amberColorMap[i].blue = uchar(i/8);
        greenColorMap[i].red = uchar(0.002*i*i+0.4405*i); greenColorMap[i].green = uchar(i);greenColorMap[i].blue = uchar(i);
        bwColorMap[i].red = uchar(i);bwColorMap[i].green = uchar(i);bwColorMap[i].blue = uchar(i);
        wbColorMap[i].red = uchar(255-i);wbColorMap[i].green = uchar(255-i);wbColorMap[i].blue = uchar(255-i);
        blueGreenRedColorMap[i].red=0;blueGreenRedColorMap[i].green=uchar(i*2);blueGreenRedColorMap[i].blue=uchar(256-i*2);
        brownColorMap[i].red = uchar(i);brownColorMap[i].green = uchar(i/1.2);brownColorMap[i].blue = uchar(i/8);
        brownWhiteColorMap[i].red = uchar(fmin(255,i*1.7));brownWhiteColorMap[i].green = uchar(fmin(255,i));brownWhiteColorMap[i].blue = 0;
    }
    for(i = 128; i < 256; i++){
        blueColorMap[i].red = uchar(i);blueColorMap[i].green = uchar(i);blueColorMap[i].blue = uchar(255);
        amberColorMap[i].red = 255;amberColorMap[i].green = uchar(66+(i-128)*1.45);amberColorMap[i].blue = uchar((256-i)/8);
        greenColorMap[i].red = uchar(0.002*i*i+0.4405*i); greenColorMap[i].green = uchar(i);greenColorMap[i].blue = uchar(i);
        bwColorMap[i].red = uchar(i);bwColorMap[i].green = uchar(i);bwColorMap[i].blue = uchar(i);
        wbColorMap[i].red = uchar(255-i);wbColorMap[i].green = uchar(255-i);wbColorMap[i].blue = uchar(255-i);
        blueGreenRedColorMap[i].red=uchar(i-128)*2;blueGreenRedColorMap[i].green=uchar(255-(i-128)*2);blueGreenRedColorMap[i].blue=0;
        brownColorMap[i].red = uchar(i);brownColorMap[i].green = uchar(i/1.2);brownColorMap[i].blue = uchar(i/8);
        brownWhiteColorMap[i].red = uchar(fmin(255,i*1.7));brownWhiteColorMap[i].green = uchar(fmin(255,i));brownWhiteColorMap[i].blue = uchar((i-128)*2);
    }
    //  Initialize more complicated maps
    for(i = 0; i < 43; i++){
        HTIColorMap[i].red = 0;HTIColorMap[i].green = 0;HTIColorMap[i].blue = uchar(80+i*4);
        BioSonicsColorMap[i].red = uchar(i*2.9);BioSonicsColorMap[i].green = 0;BioSonicsColorMap[i].blue = uchar(i*2.9);
    }
    for(i = 43; i < 86; i++){
        HTIColorMap[i].red = 0;HTIColorMap[i].green = uchar((i-43)*5.9);HTIColorMap[i].blue = 255;
        BioSonicsColorMap[i].red = uchar(255-i*2.9);BioSonicsColorMap[i].green = 0;BioSonicsColorMap[i].blue = uchar(i*2.9);
    }
    for(i = 86; i < 128; i++){
        HTIColorMap[i].red = 0;HTIColorMap[i].green = 255;HTIColorMap[i].blue = uchar(255-(i-86)*5.9);
        BioSonicsColorMap[i].red = 0;BioSonicsColorMap[i].green = uchar((i-86)*5.9);BioSonicsColorMap[i].blue = 255;
    }
    for(i = 128; i < 171; i++){
        HTIColorMap[i].red = uchar((i-128)*5.9);HTIColorMap[i].green = 255;HTIColorMap[i].blue = 0;
        BioSonicsColorMap[i].red = 0;BioSonicsColorMap[i].green = 255;BioSonicsColorMap[i].blue = uchar(255-(i-128)*5.9);
    }
    for(i = 171; i < 213; i++){
        HTIColorMap[i].red = 255;HTIColorMap[i].green = uchar(255-(i-171)*5.9);HTIColorMap[i].blue = 0;
        BioSonicsColorMap[i].red = uchar((i-171)*5.9);BioSonicsColorMap[i].green = 255;BioSonicsColorMap[i].blue = 0;
    }
    for(i = 213; i < 256; i++){
        HTIColorMap[i].red = 255;HTIColorMap[i].green = 0;HTIColorMap[i].blue = uchar((i-213)*5.9);
        BioSonicsColorMap[i].red = 255;BioSonicsColorMap[i].green = uchar(255-(i-213)*5.9);BioSonicsColorMap[i].blue = 0;
    }
    BioSonicsColorMap[0].red = 255;BioSonicsColorMap[0].green = 255;BioSonicsColorMap[0].blue = 255;
    for(i = 0; i < 64; i++){
        SimradColorMap[i].red = 0; SimradColorMap[i].green = 0; SimradColorMap[i].blue = uchar(i*3.9);
    }
    for(i = 64; i < 128; i++){
        SimradColorMap[i].red = 0; SimradColorMap[i].green = uchar((i-64)*3.9); SimradColorMap[i].blue = uchar(255-(i-64)*3.9);
    }
    for(i = 128; i < 192; i++){
        SimradColorMap[i].red = uchar((i-128)*3.9); SimradColorMap[i].green = 255; SimradColorMap[i].blue = 0;
    }
    for(i = 192; i < 256; i++){
        SimradColorMap[i].red = 255; SimradColorMap[i].green = uchar(255-(i-192)*3.9); SimradColorMap[i].blue = 0;
    }
}

//---------------------------------------------------------------
//  keyPressEvent()
//  Responds to key released events.  Currently only responds to
//  the backspace and delete key to remove marks, and the space
//  bar to start and stop playing video.
//---------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_Backspace:
        deleteSelection();
        break;
    case Qt::Key_Delete:
        deleteSelection();
        break;
    case Qt::Key_Right:
        this->nextFrame();
        break;
    case Qt::Key_Left:
        this->previousFrame();
        break;
    case Qt::Key_Return:
        this->addAnnotation(1);
        break;
    case Qt::Key_Enter:
        this->addAnnotation(1);
        break;
    case Qt::Key_1:
        this->addAnnotation(1);
        break;
    case Qt::Key_2:
        this->addAnnotation(2);
        break;
    case Qt::Key_3:
        this->addAnnotation(3);
        break;
    case Qt::Key_4:
        this->addAnnotation(4);
        break;
    case Qt::Key_5:
        this->addAnnotation(5);
        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void MainWindow::loadSettings(){
    QSettings pref("ADFG","SonarAnnotator");

    ui->threshold->setValue(pref.value("Threshold",25).toInt());
    this->changeLowerThreshold(ui->threshold->value());
    ui->intensity->setValue(pref.value("Intensity",100).toInt());
    this->changeUpperThreshold(ui->intensity->value());
    ui->colorMenu->setCurrentIndex(pref.value("Color",2).toInt());
    this->changeColorMap(ui->colorMenu->currentIndex());
    ui->backgroundRemove->setChecked(pref.value("Remove",false).toBool());
    ui->video->setRemoveBackground(ui->backgroundRemove->isChecked());
    ui->backgroundSubtract->setChecked(pref.value("Subtract",false).toBool());
    ui->video->setSubtractBackground(ui->backgroundSubtract->isChecked());
    ui->noRemoval->setChecked(pref.value("NoRemoval",true).toBool());
    ui->backgroundThreshold->setValue(pref.value("BackgroundThreshold",10).toDouble());
    this->changeBackgroundThreshold(ui->backgroundThreshold->value());
    ui->video->setBackgroundCutoff(float(ui->backgroundThreshold->value()));
    ui->averageFrames->setChecked(pref.value("AverageFrames",false).toBool());
    ui->video->averageFrames = ui->averageFrames->isChecked();
    ui->medianFrames->setChecked(pref.value("MedianFrames",false).toBool());
    ui->video->setUseMedianFilter(ui->medianFrames->isChecked());
    ui->none->setChecked(pref.value("NoSmoothing",true).toBool());
    ui->tvg->setValue(pref.value("TVG",40.0).toDouble());
    ui->alpha->setValue(pref.value("Alpha",0.0).toDouble());
    ui->category1->setText(pref.value("Category1","Fish").toString());
    ui->category2->setText(pref.value("Category2","Fish").toString());
    ui->category3->setText(pref.value("Category3","Fish").toString());
    ui->category4->setText(pref.value("Category4","Fish").toString());
    ui->category5->setText(pref.value("Category5","Fish").toString());
}

void MainWindow::open(){
    fileList = QFileDialog::getOpenFileNames(this,tr("Open Files"), currentDirectory,tr("Sonar Files (*.ddf *.jsf *.raw *.smp *.aris)"));
    openFileAtIndex(0);
}

//---------------------------------------------------------------
//  openFile()
//  Clears any existing data, then open the file passed to it.
//---------------------------------------------------------------
void MainWindow::openFile(char *name,QString type){
    if(sonarData){
        ui->echogram->setData(nullptr);
        ui->video->setData(nullptr);
        sonarData->disconnect();
        delete sonarData;
        sonarData = nullptr;        
    }
    if(type.compare("ddf")==0 || type.compare("DDF")==0){
        sonarData = new DIDSONData();
    }
    else if(type.compare("ris")==0 || type.compare("RIS")==0){      //QT only seems to recognize the last 3 characters
        sonarData = new ArisData();
    }
    else{
        QMessageBox::information( this, tr("Filetype not recognized"), tr("Filetype %1 not recognized.").arg(type),
                                  QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
    }
    if(sonarData){
        if(!strcmp(sonarData->getManufacturer(),"SMC/DIDSON") || !strcmp(sonarData->getManufacturer(),"SMC/ARIS")){
            DIDSONData *data = reinterpret_cast<DIDSONData *>(sonarData);
            data->setNumberOfBeamsToAverage(4);
            data->setUseMaximumBeam(false);
            data->setDidsonCorrection(-100);
        }
        bool result = sonarData->openFile(name);
        if(!result){
            QMessageBox::information( this, tr("Error opening file"), tr("Error opening file, try again."),
                                      QMessageBox::Ok|QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
            statusBar()->clearMessage();
            delete sonarData;
            sonarData=nullptr;
            return;
        }
        setWindowTitle(fileName);
        sonarData->setAlpha(float(ui->alpha->value()));
        sonarData->setTVG(float(ui->tvg->value()));
        ui->echogram->setData(sonarData);
        ui->video->goToFrame(ui->video->getFrameNumber()-1);
        if(!strcmp(sonarData->getManufacturer(),"SMC/DIDSON") || !strcmp(sonarData->getManufacturer(),"SMC/ARIS")){
            ui->video->setData(reinterpret_cast<DIDSONData *>(sonarData));
        }
        QString config = this->importConfiguration();       //Try to read an existing configuration
        if(config.isEmpty()){                               //If an existing configuration doesn't exist, retrieve settings from data
            config=tr("Configuration for file: %1\n").arg(QFileInfo(fileName).fileName());
            config.append(sonarData->getConfiguration());
        }
        ui->ConfigBox->setText(config);
        importCSV();
    }
}

void MainWindow::nextFrame(){
    ui->video->nextFrame();
    emit fileLocation(ui->video->getFrameNumber());
}

//---------------------------------------------------------------
//  openFileAtIndex()
//  Opens the file at the specified index if index is <= the
//  length of the file list array.
//---------------------------------------------------------------
void MainWindow::openFileAtIndex(int index){
    char *name;
    QByteArray tempByteArray;
    QString type;

    if(index>=0 && index<fileList.count()){
        statusBar()->showMessage(tr("Opening file"));
        fileIndex = index;
        fileName = fileList.at(fileIndex);
        type = fileName.right(3);
        currentDirectory = QFileInfo(fileName).path();
        tempByteArray = fileName.toLocal8Bit();
        name = tempByteArray.data();
        openFile(name,type);
        statusBar()->clearMessage();
    }
}

void MainWindow::play(){
    if(!playing){
        float frameRate = sonarData->getPingRate();
            timer->start(int(1.0/double(frameRate)*1000.0));
            playing = true;
            ui->playButton->setText("Stop");
        }
        else{
            timer->stop();
            playing = false;
            ui->playButton->setText("Play");
        }
}

void MainWindow::previousFrame(){
    ui->video->previousFrame();
    emit fileLocation(ui->video->getFrameNumber());
}

void MainWindow::receiveKeyPress(QKeyEvent *event){
    //qDebug() << "Key press received from video";
    this->keyPressEvent(event);
}

void MainWindow::saveSettings(){
    QSettings pref("ADFG","SonarAnnotator");

    pref.setValue("Threshold",ui->threshold->value());
    pref.setValue("Intensity",ui->intensity->value());
    pref.setValue("Color",ui->colorMenu->currentIndex());
    pref.setValue("Remove",ui->backgroundRemove->isChecked());
    pref.setValue("Subtract",ui->backgroundSubtract->isChecked());
    pref.setValue("NoRemoval",ui->noRemoval->isChecked());
    pref.setValue("BackgroundThreshold",ui->backgroundThreshold->value());
    pref.setValue("AverageFrames",ui->averageFrames->isChecked());
    pref.setValue("MedianFrames",ui->medianFrames->isChecked());
    pref.setValue("NoSmoothing",ui->none->isChecked());
    pref.setValue("TVG",ui->tvg->value());
    pref.setValue("Alpha",ui->alpha->value());
    pref.setValue("Category1",ui->category1->text());
    pref.setValue("Category2",ui->category2->text());
    pref.setValue("Category3",ui->category3->text());
    pref.setValue("Category4",ui->category4->text());
    pref.setValue("Category5",ui->category5->text());
}

void MainWindow::toggleBackgroundRemoval(bool value){
    if(ui->backgroundRemove->isChecked()){
        ui->video->setRemoveBackground(true);
        ui->video->setSubtractBackground(false);
    }
    if(ui->backgroundSubtract->isChecked()){
        ui->video->setRemoveBackground(false);
        ui->video->setSubtractBackground(true);
    }
    if(ui->noRemoval->isChecked()){
        ui->video->setRemoveBackground(false);
        ui->video->setSubtractBackground(false);
    }
}

void MainWindow::toggleAcrossFrameFilters(bool value){
    if(ui->averageFrames->isChecked()){
        ui->video->setAverageFrames(true);
        ui->video->setUseMedianFilter(false);
    }
    if(ui->medianFrames->isChecked()){
        ui->video->setAverageFrames(false);
        ui->video->setUseMedianFilter(true);
    }
    if(ui->none->isChecked()){
        ui->video->setAverageFrames(false);
        ui->video->setUseMedianFilter(false);
    }
}

void MainWindow::setupActions(){
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(open()));
    connect(ui->actionExport_Annotations,SIGNAL(triggered()),this,SLOT(exportAnnotations()));
    connect(ui->colorMenu,SIGNAL(currentIndexChanged(int)),this,SLOT(changeColorMap(int)));
    connect(ui->forwardButton,SIGNAL(clicked()),this,SLOT(nextFrame()));
    connect(ui->backButton,SIGNAL(clicked()),this,SLOT(previousFrame()));
    connect(ui->threshold,SIGNAL(valueChanged(int)),this,SLOT(changeLowerThreshold(int)));
    connect(ui->intensity,SIGNAL(valueChanged(int)),this,SLOT(changeUpperThreshold(int)));
    connect(ui->alpha,SIGNAL(valueChanged(double)),this,SLOT(changeAlpha(double)));
    connect(ui->tvg,SIGNAL(valueChanged(double)),this,SLOT(changeTVG(double)));
    connect(ui->backgroundRemove,SIGNAL(toggled(bool)),this,SLOT(toggleBackgroundRemoval(bool)));
    connect(ui->backgroundSubtract,SIGNAL(toggled(bool)),this,SLOT(toggleBackgroundRemoval(bool)));
    connect(ui->noRemoval,SIGNAL(toggled(bool)),this,SLOT(toggleBackgroundRemoval(bool)));
    connect(ui->backgroundThreshold,SIGNAL(valueChanged(double)),this,SLOT(changeBackgroundThreshold(double)));
    connect(ui->averageFrames,SIGNAL(toggled(bool)),this,SLOT(toggleAcrossFrameFilters(bool)));
    connect(ui->medianFrames,SIGNAL(toggled(bool)),this,SLOT(toggleAcrossFrameFilters(bool)));
    connect(ui->none,SIGNAL(toggled(bool)),this,SLOT(toggleAcrossFrameFilters(bool)));
    connect(ui->playButton,SIGNAL(clicked()),this,SLOT(play()));
    connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    connect(this,SIGNAL(fileLocation(int)),ui->echogram,SLOT(setCurrentFrameNumber(int)));
    connect(ui->echogram,SIGNAL(goToFrameNumber(int)),ui->video,SLOT(goToFrame(int)));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    connect(ui->video,SIGNAL(annotationsChanged()),dataModel,SLOT(updateData()));
    connect(ui->video,SIGNAL(keyPressedInVideo(QKeyEvent *)),this,SLOT(receiveKeyPress(QKeyEvent *)));
    connect(ui->annotationsTable->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(goToSelection(QItemSelection,QItemSelection)));
}

std::vector<int> MainWindow::uniqueFrames(AnnotationList *list){
    std::vector<int> frames;
    for(int i = 0; i < list->count(); i++){
        bool unique = true;
        if(frames.size()>0){
            for(int j = 0; j < frames.size(); j++){
                if(list->get(i)->frame==frames.at(j)){
                    unique = false;
                    break;
                }
            }
        }
        if(unique){
            frames.push_back(list->get(i)->frame);
        }
    }
    return frames;
}
