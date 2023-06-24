#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>
#include <QDebug>
#include<QKeySequence>
#include"ui_interface.h"
#include"read_file.h"
#include <iostream>
#include"rmsd.h"
#include"dialogdatset.h"
#include"global.h"
#include "dialogeditdat.h"
#include "borderdelegate.h"
#include<QHeaderView>
#include<tableitemdelegate.h>


//#include"mainwindow.h"
using namespace Eigen;
using namespace std;
namespace Ui {
class interFace;
}

class interFace : public QWidget, private  Ui::interFace
{
    Q_OBJECT

public:
    explicit interFace(QWidget *parent = nullptr);
    ~interFace();

    //####################################
    QString path ="";
    QString fname;

    //#####################################
public slots:
    void plotdata(QVector<double>, QVector<double>);



private slots:
    void setupPlot();
    void setupTable();
    void readSettings();
    void saveSettings();
    void setupSummmaryTabel();
    void setparameters();
    void setupResultsTable();
  //  void datasetchanged(const QString& );
  //  void RDboxchanged(int);
    void contextMenu(const QPoint&);
    void on_openButton_clicked();
    void getNamefromPath();
    void deletspectrum();
    void deleteAll();
    void on_pushButton_clicked();
    void ShowContextMenu(const QPoint & );
    void slotSelectionChange();
    bool eventFilter(QObject *, QEvent *);
    void stayOntab1(bool & );
    void on_backButton_clicked();
    void outPut(QList<VectorXd> );
    void outPutlabels(QStringList );
    void protIndex(MatrixXi );
    void plotRefit(VectorXd );
    void showNRMSD(double );
    void progress(int);
    void on_clearButton_clicked();
    void keyPressEvent(QKeyEvent *event);

  //  void on_copy_selectedrowButton_clicked();

void outputSummary(VectorXd &);
 void summaryContextMenu(const QPoint& pos);

    void copy();
    void copysummary();


    void on_scaleButton_clicked();

   // void on_xValbutton_clicked();
   // void XvaloutPut(QList<QList<double> > );
   // void setupXValTable();
    void NMRSDlist(QList<double> );
    void outPutProteins(QStringList );



    void on_addDatasetButton_clicked();

    void on_pushButton_2_clicked();



   // void on_editButton_clicked();
    void disable();
    void enable();

private:
    //Ui::interFace *ui;
    int plotindex=0;
    int Htindex=1;
    bool ftype;
    bool RandD=0;
    int solutionRowIndex=0;
    int protInd=0;
    int RMSDIndex=0;
    QStringList labels;
    MatrixXi protNumber;
    int progIndex=25;
    bool Xvalbool=0;
    QStringList protlist;

     int rmsdplacementindex=0;
     // QMenu myMenu;
      bool menubool=false;

 //########################
// QStandardItemModel *mModel;
// SpectraTableModel *mModel;
 QSortFilterProxyModel *pModel;



};

#endif // INTERFACE_H
