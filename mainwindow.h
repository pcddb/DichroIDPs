#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QDir>
#include<QFileDialog>
#include"mainwindow.h"
#include"read_file.h"
#include"global.h"
#include "aboutdlg.h"

namespace Ui {
class MainWindow;
}
class interFace;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    QString path ="";
    QString fname;

private slots:

    void on_actionabout_triggered();

    void on_actionHelp_triggered();

private:
    Ui::MainWindow *ui;
    interFace *IF;
};

#endif // MAINWINDOW_H



