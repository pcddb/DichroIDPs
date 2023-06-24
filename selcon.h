#ifndef SELCON_H
#define SELCON_H

//###########################################################################

//                PLEASE NOTE
//     THERE IS A CLASH OF KEYWORDS BETWEEN IGLLIB AND qT INCLUDING 'THIS' . I HAVE CHANGED IT TO THISS IN
//      SORTABLEROWS.H.
//       ALSO, NOT INCLUDING THE Q_OBJECT MACRO IN sortrows class  WHICH HAS #INCLUDE IGLIB STOPS CLASHES WITH MOC SIGNALS
//   SUCH AS  slots: and signals:. i would need to specify no keywords and pu , for eg, Q_SIGNALS: and Q_SLOTS:
/*
#define QT_NO_KEYWORDS
#define SLOT()  Q_SLOT();
#define slots  Q_SLOTS:;
#define SIGNAL()  Q_SIGNAL();
#define signals  Q_SIGNALS:;
#define emit  Q_EMIT;*/
// OR CHANGE THEM ALL BY HAND
//

#include <QObject>
#include <QApplication>
#include <QVector>
#include <QDebug>
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Jacobi>
#include<Eigen/QR>
#include<qfile.h>
#include <iostream>
#include<QtAlgorithms>
#include<QMessageBox>
#include<QProgressDialog>
#include"rmsd.h"
#include"sortrows.h"
//using namespace Eigen;
//using namespace std;

class Selcon: public QObject
{
    Q_OBJECT
public:
    explicit Selcon(QObject *parent = nullptr);
     ~Selcon();
//class Selcon
//{

public:
  //  Selcon();
    MatrixXd F;
    MatrixXd A;
    MatrixXd Fcat;
    MatrixXd Acat;
    VectorXd target;
    int maxbasis;
    QList<int> Np; //stores the number of the solution
    QList<int> Nb;// stores index of basis
   // bool RandDbool=0;

    struct selfconStruct {
        VectorXd Nprot;
        QPair<int,int> Params;
        double vecsum;
    };


    VectorXd vecAv;
    QList<double>rmsdList2;
    int runs;

Q_SIGNALS:
   // Q_SIGNAL void sendGuess(const VectorXd &);
   // Q_SIGNAL void sendHJ5Guess(const VectorXd &);
     Q_SIGNAL void sendSol(const QList< VectorXd> &);
     Q_SIGNAL void sendProteinIndex(const  MatrixXi &);
     Q_SIGNAL void sendReffitedSpec(const VectorXd &);
     Q_SIGNAL void sendNMRSD(const double &);

    //#######################################################
    //progress bar
    Q_SIGNAL void progress( const int &);

    Q_SIGNAL void Xvalprogress(const int &);
    //##############################################

    //#################################################
    // xvalidation
    Q_SIGNAL void sendSel3Sol(const QList<VectorXd> &);

    Q_SIGNAL void sendRefitlist(const VectorXd &);

public Q_SLOTS:
    void getMatrix(MatrixXd & , MatrixXd & , VectorXd & );
  //  void removeOne(MatrixXd &, MatrixXd &, bool &, bool &);
private Q_SLOTS:
    void Rmsds();
    void sortRmsds(VectorXd &);
    void sortAandF(MatrixXi &);
    void CatenateMatix(MatrixXd & , MatrixXd & );
   void Solutions();
    void sumFrac(QList<selfconStruct> &);
    void GetfinalSolutionAll(QList<selfconStruct> &);
    void output();
    void refit(QList<selfconStruct> &);
     void sel2solutions(QList<double> &, QList<selfconStruct> &);
    void Helix_rule(QList<selfconStruct> &);
    void sel3(QList<selfconStruct> &);

    void Cancel();


    VectorXd hj5();
private:
     QList<VectorXd> solutionList;
     QList<VectorXi> paramList;
     int targetIndex=0;
     QList<VectorXd>XValList;
     int XValIndex=0;
     bool XValidation=0;
     QList<double> refitList;
     bool refitbool=0;
     double refit_rule=0.0025;
     bool got_refit=0;

};

#endif // SELCON_H
