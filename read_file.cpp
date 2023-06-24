#include "read_file.h"
#include<QDebug>

Read_file::Read_file(QObject *parent) : QObject(parent)
{

}
Read_file::~Read_file()
{
    //destuctor
}
void Read_file::openSpectrum(QString & fname, bool &ftype)
{

    //######## Catch wrong file type ##########################################


    QRegExp rx1("\\.gen");
    QRegExp rx2(".txt");

    if(ftype ==0 && fname.contains(rx1)==1)
    {
        QMessageBox msgBox;
          msgBox.setText("You have checked the wrong file type.\n This is a .gen file");
          msgBox.exec();
          return;
    }
    if(ftype ==1 && fname.contains(rx2)==1)
    {
        QMessageBox msgBox;
          msgBox.setText("You have chcked the wrong file type.\n This is a .txt (x,y) file");
          msgBox.exec();
          return;
    }


//###############  determine if the file is a gen file or 2 columns x,y #######################


    if(ftype==1)
    {
        offset =7;
        wave_column=0;
        CD_column=3;
    }
    else if(ftype==0)
    {
        offset =2;
        wave_column=0;
        CD_column=1;
    }

 //###############################################
    VectorXd target;
    QStringList List, List2;

    QFile fileT(fname);

    if (!fileT.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Q not open";//Message box here
        return;
    }
    QTextStream in(&fileT);
    while (!in.atEnd()) {
        QString line = in.readLine();

        List<<line;

    }
   /* int tRows = List2.size();
    target =VectorXd(tRows);

    for(int i=0; i<List2.size();++i)
    {
        double d = List2.at(i).toDouble();
        target(i) =d;


    } */
   // qDebug()<<"list"<<List;
    extract_data(List);
}
void Read_file::extract_data(QStringList & datalist)
{
//#########################################################################

    // this function extracts the data from the .gen file

 //###############################################################################

    QStringList new_wavelength_list;
    QString wavelength;
    QMessageBox msgBox;
    // Spectra *spectra= new(Spectra);

    QRegExp rx("^[0-9]+.*");
    QRegExp exp ("\\s+");

    int index = datalist.indexOf(rx,0); //get index of the first data line

    for(int i=0; i<index; ++i)
    {

        datalist.removeFirst();  //remove the header
    }
    QString wavelength1=datalist.join(" ");
     // qDebug()<<"wlbefore"<<wavelength1;
    wavelength=wavelength1.trimmed();//removes white space
    //  qDebug()<<"wlafter"<<wavelength;

    new_wavelength_list << wavelength.split(exp);
  //  qDebug()<<"nWL"<<new_wavelength_list;

    if(new_wavelength_list.isEmpty())
    {
        msgBox.setText("Cannot read file");

        msgBox.setStandardButtons(QMessageBox::Ok );

        if(msgBox.exec() == QMessageBox::Ok)
        {

            return;
        }
    }
    else
        read_data(new_wavelength_list);
}


void Read_file::read_data(QStringList &dataList)
{

    //#########################################################################

    // this function reads the data from the .gen file

    //###############################################################################
    QVector<double> WaveLengthlist;
    QVector<double> CDlist;
   // QVector<double> HTlist;
    QVector<double> DoubledataVec;




   // int HT_column ;

    //####set column numbers##########################################

    //HT_column=2;

    foreach(const QString &s, dataList )
    {
        DoubledataVec <<s.toDouble();// data changed from Qstring to double and put into Qvector

    }

    for( int j=wave_column; j< DoubledataVec.size(); j+=offset)//select wavelength data
    {
        WaveLengthlist <<  DoubledataVec.at(j);

    }
//qDebug()<<"wldata"<<WaveLengthlist;
    for(int k=CD_column; k< dataList.size(); k+=offset ) //select cd data
    {
        CDlist<< DoubledataVec.at(k);

    }
  /*  for(int l =HT_column; l< dataList.size(); l+=offset  )//select HT data
    {


        HTlist<<  DoubledataVec.at(l);
    }*/

    //#####################################################



    //##############put in interface class

    send_data(WaveLengthlist, CDlist);
}
void Read_file::openDataSet(QString & datasets, VectorXd &Q)
{
    //##################################################################

    //                   open CD data and structure files specified by "dataset"
    //                  and read data into matrices A nd F
    //########################################################################
   // get dataset directory saved from last time prog was closed
    Global *glob;// glob is a singleton
    glob = Global::getInstance();

   // qDebug()<<"global" <<glob->globalFile;
    MatrixXd F;
    MatrixXd A;
    QStringList mList1, mList2, mList;

    Selcon *sCon = new Selcon;
    connect (sCon, SIGNAL(sendSol(QList<VectorXd>)),this,SLOT(solutions(QList<VectorXd>)),Qt::AutoConnection);
    connect (sCon, SIGNAL(sendProteinIndex(MatrixXi)),this,SLOT(getProtIndex(MatrixXi)),Qt::AutoConnection);
    connect (sCon, SIGNAL(sendReffitedSpec(VectorXd)),this,SLOT(refit(VectorXd)),Qt::AutoConnection);
    connect (sCon, SIGNAL(sendNMRSD(double)),this,SLOT(NMRSD(double)),Qt::AutoConnection);

    //################ progress bar ########################################
    connect (sCon, SIGNAL(progress(int)),this,SLOT(progress(int)),Qt::AutoConnection);



    //#########################open lbl1 (structure labels ##################

    QFile fileL(glob->globalFile + "/"+ datasets + "/lbl1.txt");
    if (!fileL.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile + "/" + datasets + "/lbl1.txt cant be found");
        msgBox.exec();
        return;
    }

    QTextStream inL(&fileL);
    while (!inL.atEnd()) {
        QString line = inL.readLine();
        labelList<<line;

    }
    labelList << "Total" <<"rmsd for self-con sol"  <<"RMSD";

    //##########################open F matrix (structures) ############

    QFile fileF(glob->globalFile +"/" + datasets + "/F.txt");
    if (!fileF.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile+"/" + datasets + "/F.txt cant be found");
        msgBox.exec();
        return;
    }

    QTextStream in(&fileF);
    while (!in.atEnd()) {
        QString line = in.readLine();
        mList1<<line;

    }
    //##############get sizes for F matrix ################################
    int fRows =mList1.size();
    QStringList col =mList1.first().split("\t");
    int fCols = col.size();

    F = MatrixXd(fRows, fCols);
    //######################################################################
    //               remove first column which contains structure labels
    //#######################################################################
    for(int i=0; i<mList1.size();++i)
    {
        QString line1 =mList1.at(i);
        QStringList firstline = line1.split("\t");

        int index=0;
        // qDebug()<<"firstline"<<firstline;
        foreach (QString s, firstline) {
            double d = s.toDouble();
            F(i,index) =d;
            ++index;
        }

    }


    //########################## openA matrix: spectra ###############

    QFile fileA(glob->globalFile +"/" + datasets +"/A.txt");
    if (!fileA.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile +"/" + datasets + "/A.txt cant be found");
        msgBox.exec();
        return;
    }

    QTextStream in2(&fileA);
    while (!in2.atEnd()) {
        QString line = in2.readLine();
        mList2<<line;

    }
    //##############get sizes for A matrix ################################
    int aRows =mList2.size();
    QStringList col2 =mList2.first().split("\t");
    int aCols = col2.size();

    A = MatrixXd(aRows, aCols);
    //######################################################################
    for(int i=0; i<mList2.size();++i)
    {
        QString line1 =mList2.at(i);
        QStringList firstline = line1.split("\t");

        int index=0;

        foreach (QString s, firstline) {
            double d = s.toDouble();
            A(i,index) =d;
            ++index;
        }

    }

    VectorXd Q1(A.rows());
    MatrixXd A1(Q.size(), A.cols());

    if (Q.size() < A.rows())
    {

        A1=A.topRows(Q.size());

        sCon->getMatrix(A1, F, Q);

    }
    else if(Q.size() > A.rows())
    {
        QString Asize;
        Asize=QString::number(241-A.rows());
        QMessageBox msg;
        msg.setText("Using lowest wavelength of dataset: "+ Asize +"nm");
        msg.exec();
        Q1=Q.head(A.rows());

        sCon->getMatrix(A, F, Q1);
    }
    else
    {

        sCon->getMatrix(A, F, Q);//send datasets here
    }
     //for error if no lbl2 file
}
void Read_file::solutions(QList<VectorXd> solution)
{
    cout<<endl<<"guess in solution"<<solution.first()<<endl;

    emit sendlabels(labelList);
    emit sendSolution(solution);
    emit sendProtInd(protIndex);
   // qDebug()<<"paramlistsize in readfile emit"<<params.size();


}
void Read_file::getProtIndex(MatrixXi protInd)
{
    protIndex = protInd;
   // qDebug()<<"readfile protind";
}

void Read_file::refit(VectorXd refitspec)
{
    emit sendRefit(refitspec);
}
void Read_file::NMRSD(double d)
{
    emit sendNMRSD(d);
}
QString Read_file::openProtLabel(QString &dataset, int & index)
{
    Global *glob;// glob is a singleton
    glob = Global::getInstance();

    QString line;
    QFile file(glob->globalFile  + "/" +  dataset +"/lbl2.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile  +"/" + dataset + "/lbl2.txt cant be found");
        msgBox.exec();
        QStringList linestring;
        for(int i=0; i<index; ++i)//fill line with column number entries
              linestring<<"no label file";

        line=linestring.join("\t");
        return line;

    }

    QTextStream in(&file);
    while (!in.atEnd()) {
         line = in.readLine();
       // List<<line;

    }
    return line;
}
void Read_file::progress( int prog)
{
    emit sendProg(prog);
}
void Read_file::XValfiles(QString & datasets)
{
    //##################################################################

    //                   open CD data and structure files specified by "dataset"
    //                  and read data into matrices A nd F
    //########################################################################

    Global *glob;// glob is a singleton
    glob = Global::getInstance();
    MatrixXd A;
    QStringList mList1, mList2, mList;

    Selcon *selCon = new Selcon;
    connect (selCon, SIGNAL(sendSel3Sol(QList<VectorXd>)),this,SLOT(XValsolutions(QList<VectorXd>)),Qt::AutoConnection);
   // connect (sCon, SIGNAL(sendProteinIndex(MatrixXi)),this,SLOT(getProtIndex(MatrixXi)),Qt::AutoConnection);
    connect (selCon, SIGNAL(sendRefitlist(VectorXd)),this,SLOT(refitList(VectorXd)),Qt::AutoConnection);
  //  connect (sCon, SIGNAL(sendNMRSD(double)),this,SLOT(NMRSD(double)),Qt::AutoConnection);

    //################ progress bar ########################################
  // connect (selCon, SIGNAL(Xvalprogress(int)),this,SLOT(progress(int)),Qt::AutoConnection);



    //#########################open lbl1 (structure labels ##################

    QFile fileL(glob->globalFile + "/" + datasets + "/lbl1.txt");
  //  qDebug()<<"filename"<<glob->globalFile << "/" << datasets << "/lbl1.txt";
    if (!fileL.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile + "/" +  datasets + "/lbl1.txt cant be found");
        msgBox.exec();
        return;
    }

    QTextStream inL(&fileL);
    while (!inL.atEnd()) {
        QString line = inL.readLine();
        labelList<<line;

    }
   // labelList << "Total" <<"rmsd for self-con sol"  <<"RMSD";

    //##########################open F matrix (structures) ############

    QFile fileF(glob->globalFile + "/" + datasets + "/F.txt");
    if (!fileF.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile  + "/" + datasets + "/F.txt cant be found");
        msgBox.exec();
        return;
    }


    QTextStream in(&fileF);
    while (!in.atEnd()) {
        QString line = in.readLine();
        mList1<<line;

    }
    //##############get sizes for F matrix ################################
    int fRows =mList1.size();
    QStringList col =mList1.first().split("\t");
    int fCols = col.size();

    FXval = MatrixXd(fRows, fCols);

    //######################################################################
    //               remove first column which contains structure labels
    //#######################################################################

    //######################################################################
    QList<int> lineSizeM; //for checking that all rows are the same size and matrix is consistent
    //error check 1 08/04/2020
    //###################################################
    for(int i=0; i<mList1.size();++i)
    {
        QString line1 =mList1.at(i);
        QStringList firstline = line1.split("\t");

        //08/04/2020
        // qDebug()<<"firstline" <<firstline; // for error check1
        int linecount =firstline.size();
        lineSizeM<<linecount;
        //  qDebug()<<"linecount" <<linecount;
        for(int i=0; i < lineSizeM.size();i++)
        {
            //   qDebug()<<"linesize"<< lineSize.at(i);
            if(lineSizeM.at(i) != linecount)
            {

                QMessageBox msgBox;
                msgBox.setText("Something wrong with Structure matrix.\n Please check via edit function");
                msgBox.exec();
                return;
            }
        }
        //##################################################################################

        int index=0;
        // qDebug()<<"firstline"<<firstline;
        foreach (QString s, firstline) {
            double d = s.toDouble();
            FXval(i,index) =d;
            ++index;
        }

    }


    //########################## openA matrix: spectra ###############

    QFile fileA(glob->globalFile  + "/" + datasets +"/A.txt");
    if (!fileA.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile + "/" + datasets + "/A.txt cant be found");
        msgBox.exec();
        return;
    }

    QTextStream in2(&fileA);
    while (!in2.atEnd()) {
        QString line = in2.readLine();
        mList2<<line;

    }
    //##############get sizes for A matrix ################################
    int aRows =mList2.size();
    QStringList col2 =mList2.first().split("\t");
    int aCols = col2.size();

    A = MatrixXd(aRows, aCols);
    //######################################################################
    QList<int> lineSize; //for checking that all rows are the same size and matrix is consistent
    //error check 1 08/04/2020
    //###################################################
    for(int i=0; i<mList2.size();++i)
    {
        QString line1 =mList2.at(i);
        QStringList firstline = line1.split("\t");
        //08/04/2020
        // qDebug()<<"firstline" <<firstline; // for error check1
        int linecount =firstline.size();
        lineSize<<linecount;
        //  qDebug()<<"linecount" <<linecount;
        for(int i=0; i < lineSize.size();i++)
        {
            //   qDebug()<<"linesize"<< lineSize.at(i);
            if(lineSize.at(i) != linecount)
            {

                QMessageBox msgBox;
                msgBox.setText("Something wrong with CD data matrix.\n Please check via edit function");
                msgBox.exec();
                return;
            }
        }
 //##################################################################################

        int index=0;
        // qDebug()<<"firstline"<<firstline;
        foreach (QString s, firstline) {
            double d = s.toDouble();
            A(i,index) =d;
            ++index;
        }

    }

    QFile fileP(glob->globalFile  + "/" + datasets + "/lbl2.txt");
    if (!fileP.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(glob->globalFile + "/" + datasets + "/lbl1.txt cant be found");
        msgBox.exec();
        return;
    }

    QTextStream inP(&fileP);
    while (!inP.atEnd()) {
        QString line = inP.readLine();
        protList<<line.split("\t");

    }




    emit sendlabels(labelList);
   // selCon->removeOne(A,FXval, RD, XValbool);
}
void Read_file::XValsolutions(QList<VectorXd>  sel3sols)
{
    QList<QList<double>> reslist;
    QList<MatrixXd> matlist;
    QList<double>Plist;
    MatrixXd Xvalmat(sel3sols.first().size(),sel3sols.size());
    for(int i=0; i<sel3sols.size(); ++i)
    {
        Xvalmat.col(i)=sel3sols.at(i);
    }
    matlist<<Xvalmat<<FXval;
    //
    Rmsd *r = new Rmsd;

    reslist=  r->rmsdXval(matlist);
    Plist= r->pearsons(matlist);// SEND TO PEARSONS HERE
   reslist.prepend(Plist);


    emit sendXVal(reslist);
}
void Read_file::refitList(VectorXd RefitNRMSD)
{
    // sort the NMRSD and proteinlist and send to table

    MatrixXi indexSort(RefitNRMSD.size(),2);
    // VectorXd RefitSort(RefitNRMSD.size());
    QList<double> RefitSort;
    QList<QString> protSort;
    sortRows *SR =new sortRows;
    indexSort= SR->sort(RefitNRMSD);
    for(int j=0; j<RefitNRMSD.size(); ++j)
    {
        int w=indexSort(j,1);
        RefitSort<<RefitNRMSD(w);
        protSort<<protList.at(w);

    }

    emit  xValNMRSDlist(RefitSort);
    emit proteinList(protSort);
}
