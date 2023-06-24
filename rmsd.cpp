#include "rmsd.h"
//################################################################################

//This class contains extra  miselanous maths as well as rmsd calculations
//###########################################################################
Rmsd::Rmsd()
{

}
QList<double> Rmsd::Rmsd1(MatrixXd & A, VectorXd & target )
{

    //######################################################################################
    //  calculates rmsd between susessive columns of a martix and a vector (target spectrum)
    //      returns values in a list
    //###################################################################################
    QList<double> rmsdList;

    for(int i=0; i<A.cols();++i)
    {

        double rmsd1=0;
        double rmsd2=0;
        for(int j=0; j<A.rows();++j)
        {
            rmsd1= rmsd1+ ( (A.col(i).row(j))-(target.row(j)))*((A.col(i).row(j))-(target.row(j))  );

        }
        rmsd2= sqrt(rmsd1/A.rows());
        rmsdList<<rmsd2;
    }


    return rmsdList;
}
double Rmsd::Rmsd2(MatrixXd & A, VectorXd & target )
{
    //######################################################################################
    //  calculates rmsd between first column of a martix and a vector (target spectrum)
    //      returns rmsd value
    //###################################################################################

    double rmsd1=0;
    double rmsd2=0;



    for(int j=0; j<target.size();++j)
    {
        rmsd1= rmsd1+ ( (target.row(j))-(A.col(0).row(j)))*((target.row(j)) -(A.col(0).row(j)) );

    }
    rmsd2= sqrt(rmsd1/A.rows());



return rmsd2;
}
double Rmsd::Rmsd3(VectorXd & A, VectorXd & target )
{
    //######################################################################################
    //  calculates rmsd between 2vectors do i need it
    //###################################################################################

    double rmsd1=0;
    double rmsd2=0;



    for(int j=0; j<target.size();++j)
    {
        rmsd1= rmsd1+ ( (target.row(j))-(A.row(j)))*((target.row(j)) -(A.row(j)) );

    }
    rmsd2= sqrt(rmsd1/A.rows());



return rmsd2;
}
double Rmsd::Rmsd4(VectorXd & A, VectorXd & target )
{
    //######################################################################################
    //  calculates NRMSD between 2vectors. This needs sorting I think
    //I think this is right. agreees with bestsel but not dichroweb 03/09/21
    //###################################################################################

    double rmsd1=0;

    double NMRSD2 =0;
    double j_squared=0;


//sqrt(sum (exp-calc)srd/(sum calc)srd
//cout<<"target-A"<<endl<<target<<"A" <<A;//if these are the same get infinitesimal nmrsd. problem here04/09/2021

    for(int j=0; j<target.size();++j)
    {
      //  cout << "A(j) is "<<A(j)<<endl;
     //   cout <<"target(j) is "<< target(j) <<endl;

        j_squared= j_squared+ ((A(j)*A(j)));

       // cout<< "j_squared is" << j_squared <<endl;
        // j_squared= j_squared+ (A(j));


        rmsd1=rmsd1+(( ( (target(j))-(A(j)))*((target(j)) -(A(j)) )));
    //qDebug()<< "jsquares"<<j_squared;
    // << "target-a" <<  target(j)-A(j)<<endl;

    //qDebug()<< "rmsd1 HERE "<<rmsd1;
    }

    NMRSD2= sqrt((rmsd1)/(j_squared));

    NMRSD2=  roundf(NMRSD2*10000)/10000; //round off to 4dp
    //qDebug()<< "rmsd1 HERE"<<rmsd1;
    return NMRSD2;
}
QList<QList<double>> Rmsd::rmsdXval(QList<MatrixXd> &list)
{
  QList<double> zlist;
  QList<double> rmsdlist;
  QList<QList<double>> doubleList;
    //########################################################################
    //  function calculates rmsd for x validation
    // ############################################################
    MatrixXd  target(list.first().rows(), list.first().cols());
    target = list.first();
    MatrixXd  Fmat(list.last().rows(), list.last().cols());
    Fmat = list.last();

    VectorXd rmsd1Vec(target.cols());
    for(int r=0; r<target.rows();++r)
    {
        double rmsd1=0;
        double std =0;
        double z=0;
        for(int c=0; c<target.cols(); ++c)

        {
            rmsd1 =rmsd1 + (target(r,c)-Fmat(r,c))*(target(r,c) -Fmat(r,c) );

        }
        double rmsd2 = sqrt(rmsd1/target.cols());

        std = Stdev(Fmat,r);
        z = std/rmsd2;
        zlist<<z;
        rmsdlist<<rmsd2;
    }

    doubleList<<rmsdlist<<zlist;
    return doubleList;
}
double Rmsd::Stdev(MatrixXd &Fmatrix,int & r)
{
    // calculates standard deviation of dssp structures ( for z value)
    double mean =Fmatrix.row(r).mean();
    double error=0;
    for (int i=0; i<Fmatrix.cols(); ++i)
    {
        error = error+ (Fmatrix(r,i)-mean)*(Fmatrix(r,i)-mean);

    }
    double stdev=sqrt(error/Fmatrix.cols());
    return stdev;
}
QList<QVector<double>> Rmsd::sortOutInterval(QVector<double> &WL,QVector<double> & CD)
{

    //############################################################################################
    //               this function converts spectra of with interval 0.5nm, tointerval of 1nm
    //                      and removes data above 240 nm
    //#################################################################################################
    QList<QVector<double>> list;
    QVector<double> WL1;
    QVector<double> CD1;



    // double lowWL = WL.first();
    // double highWL = WL.last();
    double interval =WL.at(0)-WL.at(1);
    // qDebug()<<"interval "<<interval;
    //qDebug()<<"WL "<<WL;


    if(interval==0.5)
    {
        for(int i=0;i<WL.size(); i+=2)
        {

            WL1<<WL.at(i);
            CD1<<CD.at(i);

        }
        // chop off all above 240nm
       // int size =WL1.size();
        int remove = WL.first()-240;

        for(int i= 0; i<remove; ++i)
        {
            WL1.pop_front();
            CD1.pop_front();

        }

        list <<WL1 << CD1;

        // qDebug()<<"WL1 "<<WL1;
        return(list);
    }
    else
    {
        //int size =WL.size();
        int remove = WL.first()-240;
  qDebug()<<"remove "<<remove;
        for(int i= 0; i<remove; ++i)
        {
            WL.pop_front();
            CD.pop_front();

        }
        list <<WL << CD;
        return (list);
    }
}
QList<double> Rmsd::pearsons(QList<MatrixXd> & list)
{
    MatrixXd  target(list.first().rows(), list.first().cols());
    target = list.first();
    MatrixXd  Fmat(list.last().rows(), list.last().cols());
    Fmat = list.last();

    VectorXd targetVec(target.cols());
    VectorXd FmatVec(Fmat.cols());
    QList<double> pearsonList;

    for(int r=0; r<target.rows();++r)
    {

        real_1d_array x;
        x.setlength(target.cols());
        real_1d_array y;
        y.setlength(Fmat.cols());


        for(int c=0; c<x.length();++c)
        {
            x(c)=target(r,c);
            y(c)=Fmat(r,c);

        }

        double v;

       v = pearsoncorr2(x, y);

        pearsonList<<v;

    }
    qDebug()<<"pearson"<<pearsonList;
    return pearsonList;

}


