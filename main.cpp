#include "mainwindow.h"
#include <QApplication>
#include<QDesktopWidget>
#include<QSize>
#include<QSettings>
#include <QCheckBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;



    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width(); // get width of screen
    screenHeight = desktop->height(); // get height of screen

    windowSize = w.size(); // size of our application window
    width = windowSize.width();
    height = windowSize.height();

    // little computations
    x = (screenWidth - width) / 2;
  //  y = (screenHeight-height) /2;
     y = (height)/4 ;
    //y -= 50;

    // move window to desired coordinates
    w.move ( x, y );
 //############################################################
    // dialog for first time the app is opened######################################
         bool State=1;
         QMessageBox msg;
        // QPushButton *cancelButton = msg.addButton(QMessageBox::Cancel);

         QSettings s("FirstTime", "Check-Box status");
       //  s.clear();
         QVariant Hidden = s.value("check_box_status",State);
         QCheckBox *cb = new QCheckBox("Dont show this again");
         msg.setCheckBox(cb) ;
         msg.setIcon(QMessageBox::Information);

         msg.setWindowTitle("Initialising the App");
         msg.setInformativeText(" If when you first use the app the following message appears when you click analyse:\n"

          " C//….//IDP175/lbl1.txt can’t be found \n"

          " It means that the application cannot find the dataset directory. In this case:\n"

          " 1. click the minus sign to the right of the dataset listbox to remove the entry\n"

          " 2.  Click on the + sign next to the list box.\n"

          " 3.  On the dialog click browse.\n"

          " 4.  Navigate to the dataset folder (../DichroID/datasets/ ).\n"

          " 5.  Open the dataset folder and select IDP175, BUT DONT OPEN IT.\n"

          " 6.  Click on 'select folder’ then click ‘Ok’.\n"

          " 7.  Repeat to add the SP175 dataset.\n"

          " 8.  These instructions can be found in help. ");


         if (Hidden==1) {

             msg.exec();

             if( cb->isChecked()){
                 State=0;
                 s.setValue("check_box_status", State);


             }

         }
 //########################################################################################

    w.show();


    return a.exec();
}
