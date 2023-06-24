#ifndef RMSD_H
#define RMSD_H
#include <Eigen/Dense>
#include<QList>
#include<QDebug>
#include <iostream>
#include<QMessageBox>


#include"alglib/cpp/src/stdafx.h"

//#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include "alglib/cpp/src/statistics.h"
using namespace Eigen;
using namespace std;
using namespace alglib;

class Rmsd
{
public:
    Rmsd();

public slots:

    // rmsd functions
    QList<double> Rmsd1(MatrixXd & , VectorXd &  );
    double Rmsd2(MatrixXd & , VectorXd &  );
    double Rmsd3(VectorXd & , VectorXd &  );
    double Rmsd4(VectorXd & , VectorXd &  );
    QList<QList<double> > rmsdXval(QList<MatrixXd> &);
   QList<double> pearsons(QList<MatrixXd> &);

    // other maths functions
    QList<QVector<double>> sortOutInterval(QVector<double> &,QVector<double> & );
private slots:
    double Stdev(MatrixXd &, int &);
};

#endif // RMSD_H
