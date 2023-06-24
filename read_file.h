#ifndef READ_FILE_H
#define READ_FILE_H

#include <QObject>
#include<QFile>
#include<QFileDialog>
#include<QString>
#include<QDir>
#include <Eigen/Dense>
#include<QTextStream>
#include<QMessageBox>
#include"selcon.h"
#include"global.h"

using namespace Eigen;
class Read_file: public QObject
{
    Q_OBJECT
public:
   explicit Read_file(QObject *parent = nullptr);
    ~Read_file();

    //signals:
Q_SIGNALS:
    Q_SIGNAL void send_data( const QVector<double> & ,const QVector<double> & );
    Q_SIGNAL void  sendSolution(const QList< VectorXd> &);
    Q_SIGNAL void sendlabels(const QStringList &);
    Q_SIGNAL void sendProtInd(const MatrixXi &);
    Q_SIGNAL void sendparams(const QList<VectorXi> &);
    Q_SIGNAL void sendRefit(const VectorXd &);
    Q_SIGNAL void sendNMRSD(const double&);
    Q_SIGNAL void sendXVal(const QList<QList<double>> &);
   Q_SIGNAL void xValNMRSDlist(const QList<double> &);
   Q_SIGNAL void proteinList(const QStringList &);

    //#####################################################
    //progress bar
   Q_SIGNAL void sendProg(const int &);
    //#############################################
public Q_SLOTS:
    void openSpectrum(QString &, bool &);
    void openDataSet(QString &, VectorXd &Q);
    void solutions(QList<VectorXd> );
    void XValfiles(QString &);

    QString openProtLabel(QString &, int &);
private Q_SLOTS:
    void extract_data(QStringList &);
    void read_data(QStringList &);
    void getProtIndex(MatrixXi );
   // void getParams(QList<VectorXi> );
    void refit(VectorXd );
    void NMRSD(double);
    void progress( int );
    // for xvalidation
    void XValsolutions(QList<VectorXd> );
    void refitList(VectorXd );
private:
    int offset ;
    int wave_column ;
    int CD_column ;
    QStringList labelList;
    MatrixXi protIndex;
    QList <VectorXi> params;
    int col=1;
    MatrixXd sel3solutions;
    MatrixXd FXval;
    QStringList protList;



};

#endif // READ_FILE_H
