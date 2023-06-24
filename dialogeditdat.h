#ifndef DIALOGEDITDAT_H
#define DIALOGEDITDAT_H

#include <QDialog>
#include <QObject>
#include<QDir>
#include<QFileDialog>
#include<QMessageBox>
#include<QTextStream>
#include<QDebug>
#include<QMenu>
#include<Eigen/Dense>
#include <iostream>
#include<QStandardItem>
#include<QSortFilterProxyModel>
#include<QClipboard>
#include<QKeyEvent>
#include<QInputDialog>
#include"spinboxdelegate.h"
#include"global.h"

using namespace Eigen;
namespace Ui {
class DialogEditDat;
}

class DialogEditDat : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditDat(QWidget *parent = nullptr);
    ~DialogEditDat();

private slots:
    void initialiseTable();
   // void horizontalHeaderClicked(int );
   // void verticalHeaderClicked(int );
    void setUpTable();

    void on_openButton_clicked();
    void getData();
    void protlistToTable(QStringList &);
    void structureLabelToTable(QStringList & );
    void FmatrixToTable(MatrixXd &);
    void AmatrixToTable(MatrixXd &Fmat);

    void on_clearButton_clicked();

    void on_closeButton_clicked();

    void ShowContextMenu(const QPoint & );
    //context menu editing

    void deleteRow();
    void deleteColumn();
    void addRow();
    void addColumn();
    //void addColumnEnd();
    void paste();
    void deleteSelection();
    void copySelection();

  //  void verticalHeaderDblClicked(int );
   // void keyReleaseEvent(QKeyEvent *Revent);

    void on_saveButton_clicked();
    void savedatafiles(QString & fileName);
    bool fileExists(QString );

private:
    Ui::DialogEditDat *ui;

     QStandardItemModel *mModel;
    // SpectraTableModel *mModel;
     QSortFilterProxyModel *pModel;
     SpinBoxDelegate *deligate;
public:
    //QString path ="";
    QString fname;
    int cols ;
    int rows=1;
    int row_offset;


};

#endif // DIALOGEDITDAT_H
