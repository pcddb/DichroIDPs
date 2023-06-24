#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    IF= new interFace;

QString dir =QCoreApplication::applicationDirPath()+"/../datasets/";
Global *glob;// glob is a singleton containing dataset file directory
glob = Global::getInstance();
glob->globalFile=dir;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionabout_triggered()
{
    aboutDlg *adlg = new aboutDlg;
    adlg->exec();
}


void MainWindow::on_actionHelp_triggered()
{
    QString dirpath =QCoreApplication::applicationDirPath()+"/../doc/DA_Help.htm";
        QUrl url;
        qDebug()<<"path"<<dirpath;
        url.setPath(dirpath);
      //  url.fromLocalFile(dirpath+"/doc/index.htm");
        // url.fromLocalFile(QFileInfo("doc/index.htm").absoluteFilePath());
        url.setScheme("file");
        if ( !QDesktopServices::openUrl(url) )
            qDebug() << "Failed to open url";
}
