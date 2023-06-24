#ifndef SORTROWS_H
#define SORTROWS_H

#include<QObject>
#include<QList>
#include<Eigen/Dense>
#include<igl/sort.h>
using namespace Eigen;

class sortRows
{
public:
    sortRows();
public slots:
    MatrixXi sort(VectorXd & );
};

#endif // SORTROWS_H
