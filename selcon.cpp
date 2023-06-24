#include "selcon.h"
using namespace Eigen;
using namespace std;
//using namespace igl;
Selcon::Selcon(QObject *parent) : QObject(parent)
{

}
Selcon::~Selcon()
{
    //destructor
}

void Selcon::getMatrix(MatrixXd & aMatrix, MatrixXd & fMatrix, VectorXd & qMatrix)
{
    A= aMatrix;
    F= fMatrix;
    target =qMatrix;
    //RandDbool = RDbool;
  //cout<<"target"<<endl<<target<<endl;
    Rmsds();
}
void Selcon::Rmsds()
{
    //###########################################################################
    //            Function calculates the rmsd between the target spectrum
    //          and each spectrum in matrix A
    //#######################################################################
    QList<double> rmsdList;
    Rmsd *rms=new Rmsd;

    rmsdList= rms->Rmsd1(A, target);

    int size =rmsdList.size();
    VectorXd rmsds(size);
    for(int i=0; i<rmsdList.size(); ++i)
    {
        rmsds.row(i)<< rmsdList.at(i);
    }

  //  cout<<"rmsds "<<endl<<rmsds<<endl;
    sortRmsds(rmsds);
}
void Selcon::sortRmsds(VectorXd &rmsds)
{
//######################################################################
    //            function sorts rmsds into ascending order
//#####################################################################
    MatrixXi indexSort(rmsds.size(),2);

    sortRows *SR =new sortRows;
    indexSort= SR->sort(rmsds);
  //  cout <<"indexsort"<<indexSort<<endl;

    //put connect statement in readfile
    Q_EMIT sendProteinIndex(indexSort);

    sortAandF(indexSort);
}
void Selcon::sortAandF(MatrixXi &indexSort)
{
//##############################################################################
   // Matricies A nd F are sorted accordingto the order of rmsds
    //######################################################################
    MatrixXd Fsort(F.rows(),F.cols());
    MatrixXd Asort(A.rows(),A.cols());
    //cout<<"indexsortrows "<<indexSort.rows()<<endl;
    //cout<<"indexsortcols "<<indexSort.cols()<<endl;
    //cout<<"Fsortrows "<<Fsort.rows()<<endl;
    //cout<<"Fsortcols "<<Fsort.cols()<<endl;

    // ################ sort A amtrix ####################
    for(int i=0; i<Asort.rows(); ++i)
    {
        for(int j=0; j<Asort.cols(); ++j)
        {
            int w=indexSort(j,1);
            Asort(i,j)=A(i,w);

        }
    }

  //   cout<<"Asort"<<endl<<Asort<<endl;

    // ################ sort F amtrix ####################

    for(int i=0; i<Fsort.rows(); ++i)
    {
        for(int j=0; j<Fsort.cols(); ++j)
        {
            int w=indexSort(j,1);
            Fsort(i,j)=F(i,w);

        }
    }

   // cout<<"fsort"<<endl<<Fsort<<endl;
    CatenateMatix(Asort, Fsort);

}
void Selcon::CatenateMatix(MatrixXd & Asort, MatrixXd & Fsort)
{
    //###############################################################################
    //              catenate target to a matrix
    //              catenate top strucure to M matrix (best guess)
    //###############################################################################

    Acat =MatrixXd(Asort.rows(),Asort.cols()+1);
    Fcat= MatrixXd(Fsort.rows(),Fsort.cols()+1);
    VectorXd Fguess(Fsort.rows());
    Fguess=Fsort.col(0);
    Fcat.col(0)=Fguess;
    Fcat.block(0,1,Fsort.rows(),Fsort.cols())=Fsort;

    Acat.col(0)=target;
    Acat.block(0,1,Asort.rows(),Asort.cols())=Asort;

    runs=0;


    // get total and put in last row of vector


    VectorXd vec2(1);
    vec2.tail(1)<< Fguess.sum();//this dont work
    VectorXd vec_joined(Fguess.size()+1);
    vec_joined<< Fguess, vec2;

    solutionList << vec_joined<<vec_joined;
   //cout<<"acat"<<endl<<Acat<<endl;
   //cout<<Fcat<<endl;

    Solutions();
}

void Selcon::Solutions()
{
    //####################################################################################
    //                    for min number of proteins to max number
    //                     for min number of basis spectra to max number
    //                   calculate solution and store in list
    //#################################################################################
    maxbasis =7;
   // int ind =0;
    QPair<int,int> params;
 //   QList<QPair<int,int>>paramlist;

    QList<VectorXd> vecList;  MatrixXd Smat;
    VectorXd vec(3);
    VectorXd solVec(Fcat.rows());
    QList<selfconStruct> selfconlist;
    for(int i=3; i<Acat.cols()+1; ++i)
    {
        // do svd on spectra matrix




        JacobiSVD<MatrixXd> svd(Acat.leftCols(i), ComputeThinU | ComputeThinV);
        // if columns < rows i need to sort u and v matices so Smat is square; cf the matlab function svd(A,0)
        // try|FullPivHouseholderQRPreconditioner if m>n only first n columns of u done and s =n*n
        //m < n — Only the first m columns of V are computed, and S is m-by-m.
     if(Acat.rows()>i|Acat.rows()==i)
        {
            Smat.resize(i,i);
            Smat =MatrixXd::Zero(i,i);
            vec.resize(i);
            vec=svd.singularValues();

        }
        //this keeps smatrix square but im not sure if its right but it seems to work
        // either way i get less solutions than matlab??
        if(Acat.rows()<i)
        {
            Smat.resize(Acat.rows(),Acat.rows());
            Smat =MatrixXd::Zero(Acat.rows(),Acat.rows());
            vec.resize(i);
            vec=svd.singularValues();
            vec.resize(Acat.rows());
        }
        //#################################################################
        Smat.diagonal()=vec;

//cout <<"vec"<<endl<<vec<<endl;

        MatrixXd pinv(vec.size(),vec.size());
        pinv = Smat.completeOrthogonalDecomposition().pseudoInverse();

        MatrixXd U(svd.matrixU().rows(),i);
        MatrixXd Ut(i,svd.matrixU().rows());
        U=svd.matrixU();

        if(i<8)//compensating for j=1
            maxbasis=i;
        //for various number ofbasis spectra get solutions
         selfconStruct *selfcon1 =new selfconStruct;

        for(int j =1;j<maxbasis+1; ++j)//NOT SURE ABOUT THE INDECIES
        {

            Ut=U.leftCols(j).transpose();

            //    cout<<"sol"<<endl<<((Fcat.leftCols(i)*svd.matrixV().leftCols(j)*pinv.topRows(j).leftCols(j))*(Ut)*target)<<endl;
            solVec<<((Fcat.leftCols(i)*svd.matrixV().leftCols(j)*pinv.topRows(j).leftCols(j))*(Ut)*target);
            vecList<<solVec;

            selfcon1->Nprot=solVec;
            //need to save i and j in an array so that the solutions can be matchedto a column in F
            // that are formed using the same basis. the best of each (closest total 1) will be averaged for  final solutions
            // then the closest solution replaces Fcat.row(0) and the processi s reiterated untill rms between succesive solutions is
            // < 0.0025
            params.first=i;
            params.second=j;
            //paramlist<<params;

            selfcon1->Params=params;

            selfconlist<<*selfcon1;

            // cout<<"  i "<<i;
            // cout<<" index "<<ind<<endl;
            //put i and j values in vectors. then put vecors and match with index of vecor i nveclist
        }
       //   cout<<"  j "<<j;
      // cout<<"  i "<<i;
    }
  //  qDebug()<<"list size"<<vecList.size();

   // qDebug()<<"nprot1size"<<selfcon.Nprot1.size();*/
//qDebug()<<"selfconlistsize"<<selfconlist.size();


    sumFrac(selfconlist);
}


void Selcon::sumFrac(QList<selfconStruct> &selfconlist)
{
    //#####################################################################
    //  function implements the sum and fract rules
    //#####################################################################
    double fractrule=-0.025;
    double sumrule =0.05;// these can be input from app IN FUTURE

//

    int frac=0;

    bool found_one=0;//if solutions are found it will be 1. if not rules are relaxed 07/04/22

    selfconStruct *selfcon2 =new selfconStruct;
    QList<selfconStruct> selfconlist2;

    //   fract rule. no fraction in any solution is <= -0.025
while (found_one==0)
{
    foreach(selfconStruct v , selfconlist)
    {

        for(int i=0; i<v.Nprot.size(); ++i)
        {
            if(v.Nprot(i) < fractrule| v.Nprot(i)==fractrule)
                frac++;
        }


        double sum = abs(( v.Nprot.sum())-1);


        if((sum<sumrule|sum==sumrule)&&frac==0)
        {
            selfcon2->Nprot=v.Nprot;

            selfcon2->Params=v.Params;
            selfcon2->vecsum =sum;
            // need to store the sums
            selfconlist2<<*selfcon2;
        }
        frac=0;
    }

    if(selfconlist2.empty())
    {
        qDebug()<<"no solution. Relax parameters";
        sumrule =0.005;// these can be input from app
        fractrule=-0.01;
    }
    else
    {
        qDebug()<<"sum value is" << sumrule;
        found_one=1;
    }
    if(selfconlist2.size()==1)
    {
        //send to output
           //#### NEED MESSAGE BOX HERE
        //and do something ?
        qDebug()<<"only1";
    }


}
    emit progress(10);
    GetfinalSolutionAll(selfconlist2);

}
void Selcon::GetfinalSolutionAll(QList<selfconStruct> &selfconlist2 )
{
    //#######################################################################################################################
    // in this function: the solution from each number of basis with the total closest to 1 is found
    // these solutions are then averaged and output. The solution then replaces the origonal guess in the strucure matrix
    // and the process reiterated until an rmsd between succesive solutions < 0.0025
    //#####################################################################################################
    QList<selfconStruct> selfconlist3;
    QList<selfconStruct> selfconlist4;
    VectorXd vec(Fcat.rows());

    VectorXd vecsum= VectorXd::Zero(Fcat.rows());
    vecAv=VectorXd::Zero(Fcat.rows());

    QList<double> sumlist;

    double rmsd2=0;
    int index=3;
    int index2 =0;

//####################################This is a bit of an almighty hack ##################################

  //  foreach(selfconStruct sf, selfconlist2)
       for(const selfconStruct &sf: selfconlist2)//31/03/22
    {

        if(sf.Params.first==(index))
        {

            sumlist<<sf.vecsum;
            selfconlist3<<sf;
            ++index2;
        }
        else if(sf.Params.first!=index && !sumlist.isEmpty())
        {
            //when params.first goes from (for example 3 to 4) and sumlist.size is greater than 1 we sort it
            //because the lowest number indicates the closest value to 1. (from sumfrac function)
            if(sumlist.size()>1)
            {

                std::sort(sumlist.begin(), sumlist.end());

                for(int i=0; i<selfconlist3.size(); ++i)
                {
                    //we then take the lowest
                   // qDebug()<<"selfconlist.at(i).vecsum"<<selfconlist3.at(i).vecsum;
                   // qDebug()<<"sumlist.first()"<<sumlist.first();
                    if(selfconlist3.at(i).vecsum==sumlist.first())
                    {
                       // qDebug()<<"yes";
                        selfconlist4<<selfconlist3.at(i);
                    }
                }
                ++index;
                ++index2;
                sumlist.clear();

                selfconlist3.clear();
            }
            if(sumlist.size()==1)
            {

                selfconlist4<<selfconlist3.first();

                ++index;
                ++index2;
                sumlist.clear();

                selfconlist3.clear();
            }
        }
        else if(sf.Params.first!=index && sumlist.isEmpty())
        {

            if(index2<selfconlist2.size()-1)
            {
                if(selfconlist2.at(index2).Params.first < selfconlist2.at(index2+1).Params.first )
                {
                    index=selfconlist2.at(index2+1).Params.first ;

                    ++index2;

                    selfconlist3.clear();
                }
                else if(selfconlist2.at(index2).Params.first == selfconlist2.at(index2+1).Params.first )
                {

                    selfconlist3.clear();
                    ++index2;
                }
            }

        }

    }
  /*  for(int i=0; i<selfconlist4.size();++i)
    {
        qDebug()<<"self4 "<<selfconlist4.at(i).Params;
    }*/

// ########################## end of almighty hack ##################################################

    for(int i=0; i<selfconlist4.size(); ++i)
    {
        vec =selfconlist4.at(i).Nprot;
        vecsum=vecsum+vec;

    }
    vecAv= vecsum/selfconlist4.size();


    Rmsd *rms=new Rmsd;

    rmsd2= rms->Rmsd2(Fcat, vecAv);


    Fcat.col(0) = vecAv;

    rmsdList2<<rmsd2;

    if(rmsdList2.size()==1)
    {
        ++runs;
          emit progress(runs*10);//for tracking bar

     // qDebug()<<"runs i nsol1  "<<runs;
        Solutions();
    }


  //  while (got_refit==0) //for rlaxing refit rule 07/04/22 doesnt work in xvalidation
   //{


        if(rmsdList2.size()>1)// && runs <50)//****************************************888888 31/03/22
        {
            if(abs(rmsdList2.first()-rmsdList2.last())>= refit_rule)// && abs(rmsdList2.first()-rmsdList2.last())<0.05) //RELAX HERE
            {
                //  qDebug()<<"rmsd diff inside loop"<<rmsdList2.first()-rmsdList2.last();
                rmsdList2.removeFirst();

                ++runs;
                //  qDebug()<<"runs i nsol2  "<<runs;
                emit progress(runs *10);
                refit_rule = refit_rule+0.01;
                qDebug()<<"got refit is "<< got_refit;
                Solutions();

            }


            else if (abs(rmsdList2.first()-rmsdList2.last())< refit_rule)

            {


                cout<<"rmsd for self consist attempt "<<runs<<"  is "<< rmsd2<<endl;

                //  cout<<"Selcon solutions  "<<endl<<vecAv<<endl;
                VectorXd vec2(1);
                vec2.tail(1)<<vecAv.sum();//this dont work????? it works but for what?01/04/22
                VectorXd vec_joined(vecAv.size()+1);
                vec_joined<< vecAv, vec2;
                //cout<<"vecav " <<vecAv <<endl;
                //cout<<"vec2 " <<vec2 <<endl;
                solutionList<<vec_joined;
                rmsdList2.clear();
                //  rmsd2=0;
                qDebug()<< " refit rule is" <<refit_rule;

                    got_refit=1;
                    refit_rule=0.0025;


                refit(selfconlist4);


                return;
            }


        }


  // }
    /*   else if(rmsdList2.size()>0 &&runs>=50)//****************************88888831/03/22
    {
        cout << "no convergence at 50 runs"<<endl;
        cout<<"rmsd for self consist attempt "<<runs<<"  is "<< rmsd2<<endl;
      //  cout<< "rmsd between Query and Refitted spectrum is "<< "msd4"<<endl;
        QMessageBox msgBox;
        msgBox.setText("Self consistent method produced no convergence after 50 runs\n ");
        msgBox.exec();

        VectorXd vec2(1);
        vec2.tail(1)<<vecAv.sum();//this dont work
        VectorXd vec_joined(vecAv.size()+1);
        vec_joined<< vecAv, vec2;

        solutionList<<vec_joined;
        //cout<<"Selcon solutions  "<<endl<<vecAv<<endl;

        rmsdList2.clear();
        refit(selfconlist4);
        return;
    }*/

    return;

}
void Selcon::output()
{

}
void Selcon::refit(QList<selfconStruct> & params)
{

    MatrixXd Smat;
    VectorXd vec(3);
    MatrixXd refitted(3,3);
    QList<double> refitlist;
    double refits;
    Rmsd *rms=new Rmsd;
  //  got_refit=0;

    //##### get paramaeters so the proteins used in sel1 can be displayed ##############################
   /* VectorXi sel1Prots(params.size());
    for(int i=0;i<params.size();++i)
    {

        // qDebug()<<"params  "<<endl<<params.at(i).Params<<endl;//send params :proteins used in sel1 and sent to sel2
        sel1Prots(i)=params.at(i).Params.first;

    }
    paramList<<sel1Prots;*/

    for(int i=0; i<params.size(); ++i)
    {

        int np = params.at(i).Params.first;
        int basis =params.at(i).Params.second;


        JacobiSVD<MatrixXd> svd(Acat.leftCols(np), ComputeThinU | ComputeThinV);
        MatrixXd Vt(0,0);
        Vt.resize(svd.matrixV().cols(), svd.matrixV().rows());
        Vt = svd.matrixV().transpose();


        // if columns < rows i need to sort u and v matices so Smat is square; cf the matlab function svd(A,0)
        // try|FullPivHouseholderQRPreconditioner if m>n only first n columns of u done and s =n*n
        //m < n — Only the first m columns of V are computed, and S is m-by-m.
        if(Acat.rows()>np|Acat.rows()==np)
        {
            Smat.resize(np,np);
            Smat =MatrixXd::Zero(np,np);
            vec.resize(np);
            vec=svd.singularValues();

        }
        //this keeps smatrix square but im not sure if its right but it seems to work
        // either way i get less solutions than matlab??
        if(Acat.rows()<np)
        {
            Smat.resize(Acat.rows(),Acat.rows());
            Smat =MatrixXd::Zero(Acat.rows(),Acat.rows());
            vec.resize(np);
            vec=svd.singularValues();
            vec.resize(Acat.rows());
        }

        Smat.diagonal()=vec;


        //############### Do refits #####################################

        refitted.resize(svd.matrixU().rows(),np);
        refitted = svd.matrixU().leftCols(basis)*Smat.topRows(basis).leftCols(basis)*Vt.topRows(basis);


        refits= rms->Rmsd2(refitted, target);

        //cout<<"target"<<endl<<target<<endl;
        refitlist <<refits;
    }


     sel2solutions(refitlist, params);
}
void Selcon::sel2solutions(QList<double> &refits, QList<selfconStruct> & params)
{
    double refitrule =0.25;//changed from 0.25 06/04/22
    selfconStruct sel2solutions;
    QList<selfconStruct> sel2List;
    VectorXd Sel2Sol =VectorXd::Zero(Fcat.rows());
    VectorXd avVec =VectorXd::Zero(Fcat.rows());
    VectorXd Sel2Sol2=VectorXd::Zero(Fcat.rows());


    for(int i=0; i<refits.size(); ++i)//refit then needs to be relaxed
    {
        if( refits.at(i)<=refitrule)
        {
            // this is wrong???
            sel2solutions.Nprot=params.at(i).Nprot;
            sel2solutions.Params=params.at(i).Params;
            sel2solutions.vecsum=params.at(i).vecsum;
  // qDebug()<<"sel2sol"<<endl<<sel2solutions.Params;
            sel2List<<sel2solutions;
        }

    }
    //####  MESSAGE BOX HERE
    if(sel2List.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("No Selcon2 solutions ");
        msgBox.exec();

       // qDebug()<<"no sel2 solutions";
        return;
    }


  //  foreach(selfconStruct sel, sel2List)
   for(const selfconStruct &sel: sel2List)//31/03/22
    {
        for(int i=0; i<sel.Nprot.cols(); ++i)
        {
            avVec=  sel.Nprot.col(i);
            Sel2Sol+=avVec;
             //cout<<"avvec"<<endl<<avVec<<endl;
        }
    }
    Sel2Sol2=Sel2Sol/sel2List.size();

    // get total, put this in last row of selvector and put in list for output later

    VectorXd vec2(1);
    vec2.tail(1)<<Sel2Sol2.sum();//this dont work
    VectorXd vec_joined(Sel2Sol2.size()+1);
    vec_joined<< Sel2Sol2, vec2;

    solutionList<<vec_joined;

// SEL2LIST CONTAINS INDEX OF PROTEINS IN ACAT USED IN SEL2
    //PUT INDEX IN PARAMLIST TO BE SENT TO OUTPUT
   /* VectorXi sel2Prots(sel2List.size());
    for(int i=0;i<sel2List.size();++i)
    {

        // qDebug()<<"params  "<<endl<<params.at(i).Params<<endl;//send params :proteins used in sel1 and sent to sel2
        sel2Prots(i)=sel2List.at(i).Params.first;

    }
    paramList<<sel2Prots;*/
qDebug()<<"sel2List.size() "<<sel2List.size();
    // now do selcon3
    Helix_rule(sel2List);

}
void Selcon::Helix_rule(QList<selfconStruct>  &sel2List)
{
//###########################################################################################
    //in this function the helix rule is applied to find selcon3 solutions
//##########################################################################################
    QList<double> hel_list;
    QList<selfconStruct> sel3sol;

    VectorXd Hj5Pred(F.rows());

    double helix;
    double Hel;
    double Hjh;
    double hel_av=0;

    Hj5Pred= hj5();
  //  cout<<"HJ5"<<endl<<Hj5Pred<<endl;

    VectorXd vec2(1);
    vec2.tail(1)<<Hj5Pred.sum();//this dont work
    VectorXd vec_joined(Hj5Pred.size()+1);
    vec_joined<< Hj5Pred, vec2;
    solutionList.replace(1,vec_joined);
    refitbool=0;

    Hjh = Hj5Pred(0);

    for(int i=0; i<sel2List.size(); ++i)
    {
        //  cout<<"list"<<endl<< sel2List.at(i).Nprot.row(0) +sel2List.at(i).Nprot.row(1)<<endl;
        Hel=sel2List.at(i).Nprot(0);// +sel2List.at(i).Nprot(1);

        hel_list<<Hel;
    }


    if(Hjh>0.65)
    {
        for(int i=0; i<sel2List.size(); ++i)
        {

            helix= hel_list.at(i);
            if(helix>0.65)
                sel3sol<<sel2List.at(i);
        }
    }

    std::sort(hel_list.begin(), hel_list.end());//hel max= hel_list.last(). helmin is hel_list.first()

 //qDebug()<<"hellist"<<endl<<hel_list<<endl;
// qDebug()<<"Hjh" <<Hjh;
// qDebug()<<"helistfisrt+"<<((Hjh+hel_list.first())/2 +0.03);
// qDebug()<<"helistfisrt-"<<((Hjh+hel_list.first())/2 -0.03);

 //it all works 08/04/2022
    if(Hjh<=0.65 && Hjh>=0.25)
    {
        for(int i=0; i<sel2List.size(); ++i)
        {

            helix= hel_list.at(i);
            if(helix<=(((Hjh+hel_list.last())/2)+0.03) &&helix>=(((Hjh+hel_list.last())/2)-0.03))
                sel3sol<<sel2List.at(i);
        }

    }
    for(int i=0; i<hel_list.size(); ++i)
    {
        hel_av =hel_av+ hel_list.at(i);
    }

    hel_av/=hel_list.size();

    if(Hjh<0.25 && Hjh>=0.15)
    { for(int i=0; i<sel2List.size(); ++i)
        {

            helix= hel_list.at(i);
            if(helix<=(((Hjh+hel_av)/2)+0.03) && helix>=(((Hjh+hel_av)/2)-0.03))
                sel3sol<<sel2List.at(i);
        }

    }
    if(Hjh<0.15 )
    {
        for(int i=0; i<sel2List.size(); ++i)
        {

            helix= hel_list.at(i);
            if(helix<=(((Hjh+hel_list.first())/2)+0.03) && helix>=(((Hjh+hel_list.first())/2)-0.03))
                sel3sol<<sel2List.at(i);
        }
    }
    qDebug()<<"sel3soll after helix rule"<< sel3sol.size();
    if(sel3sol.size()<1)
    {
      // if(XValidation==0)//###################//07/04/22  allows sel2 solutions to show in results
       // {
            QMessageBox msgBox;
            msgBox.setText("No Selcon3 solutions. Using sel2 solutions");
            msgBox.exec();
              qDebug()<<" No Sel3 solution, using sel2 solutions";
            // get mean of sel2 solutions for next bit need to sort out an rmsd for this o
            //or not produce one. use a bool which stops rmsd calculation.

         //   refitbool=0;

         //   Q_EMIT sendSol(solutionList);
          //  return;
      //  }
      //  else
       // }
            qDebug()<<"ere in sel2 empty xval";
            sel3sol=sel2List;
            refitbool =1;
            // need to signal that the rmsd is wrong. use a bool

        //}

    }

 sel3(sel3sol);

}
void Selcon::sel3(QList<selfconStruct> &sel3solList)
{
    VectorXd Sel3Sol =VectorXd::Zero(Fcat.rows());
    VectorXd avVec =VectorXd::Zero(Fcat.rows());
    VectorXd Sel3Sol2=VectorXd::Zero(Fcat.rows());
    QList<QPair<int,int>> refit_np_param;
    int np, basis;
//SEL3SOLLIST IS PROTEINS USED IN SEL3 AFTER HELIX RULE

VectorXi sel3Prots(sel3solList.size());
for(int i=0;i<sel3solList.size();++i)
{
    sel3Prots(i)=sel3solList.at(i).Params.first;

}
paramList<<sel3Prots;

for(const selfconStruct &sel: sel3solList)
{

    for(int i=0; i<sel.Nprot.cols(); ++i)
    {
        avVec=  sel.Nprot.col(i);
        Sel3Sol+=avVec;
        refit_np_param<<sel.Params;
        // cout<<avVec<<endl;
    }
    }
    Sel3Sol2=Sel3Sol/sel3solList.size();

    //put total in last row of sel3 vector and add to list

    VectorXd vec2(1);
    vec2.tail(1)<<Sel3Sol2.sum();//this dont work
    VectorXd vec_joined(Sel3Sol2.size()+1);
    vec_joined<< Sel3Sol2, vec2;
    cout<<"sel3 solution"<<endl<<Sel3Sol2<<endl;
    solutionList<< vec_joined;

    Q_EMIT sendSol(solutionList);

    XValList<<Sel3Sol2;//make this a matrix

    //for xval results
    ++XValIndex;

    if(XValIndex==A.cols()+1)
      //  if(XValIndex==3)
    {
       // qDebug()<<"ere xval";
        Q_EMIT sendSel3Sol(XValList);//this is sent when its all over
    }

    //############### refit selcon solutions ###############################
    MatrixXd Smat;
    VectorXd vec(3);
    MatrixXd Vt(0,0);
    MatrixXd refit(0,0);
    VectorXd refit_prot(A.rows());
    QList<VectorXd> refit_prot_list;
    double rmsdRefit;
    for(int i=0; i<refit_np_param.size(); ++i)
    {
        np=refit_np_param.at(i).first;
        basis =refit_np_param.at(i).second;

        JacobiSVD<MatrixXd> svd(Acat.leftCols(np), ComputeThinU | ComputeThinV);

        if(Acat.rows()>np|Acat.rows()==np)
        {
            Smat.resize(np,np);
            Smat =MatrixXd::Zero(np,np);
            vec.resize(np);
            vec=svd.singularValues();

        }
        //this keeps smatrix square but im not sure if its right but it seems to work
        // either way i get less solutions than matlab??
        if(Acat.rows()<np)
        {
            Smat.resize(Acat.rows(),Acat.rows());
            Smat =MatrixXd::Zero(Acat.rows(),Acat.rows());
            vec.resize(np);
            vec=svd.singularValues();
            vec.resize(Acat.rows());
        }
        Smat.diagonal()=vec;

        // not in sorens version#####################################
    //  dont neeed pinv: messes it right up
//##########################################################################
        Vt.resize(svd.matrixV().cols(), svd.matrixV().rows());
        Vt= svd.matrixV().transpose();




        refit.resize(svd.matrixU().rows(),Vt.cols());
        refit= svd.matrixU().leftCols(basis)*Smat.topRows(basis).leftCols(basis)*Vt.topRows(basis);
        refit_prot=refit.col(0);
        refit_prot_list<<refit_prot;
    }
    // average refited protein spectra ###############################
    VectorXd refit_prot2=VectorXd::Zero(Acat.rows());

     refit_prot2=refit_prot.rowwise().mean();

   // cout<<"refit_prot2"<<endl<<refit_prot2<<endl;
    Rmsd *rms =new Rmsd;
    rmsdRefit=rms->Rmsd4(refit_prot2, target); //RMSD is rms4
    cout<< "RMSD between query and rfitted spectrum "<<rmsdRefit<<endl;
//##########################################################################
    // send list of NMRSDS for xval results
//qDebug()<<"refitbool"<<refitbool;
  //  if(refitbool==1)
   //     refitList<<0;// if no sel3 solution but NAN in refitlist
   // else
    refitList<<rmsdRefit;
   /* if(XValIndex==A.cols()+1)
      //  if(XValIndex==3)
    {
        VectorXd RefitVec(refitList.size());
        for(int i=0; i<A.cols()+1;++i)//######################
        {

            RefitVec.row(i)<<refitList.at(i);

        }

     Q_EMIT sendRefitlist(RefitVec);
    }*/
//#############################################################
 //   this->blockSignals(true);
    cout<<"refit_prot2"<<endl<<refit_prot2<<endl;

    Q_EMIT sendReffitedSpec(refit_prot2);
    qDebug()<<"rmsdrefit"<<rmsdRefit;
    Q_EMIT sendNMRSD(rmsdRefit);

}
VectorXd Selcon::hj5()
{
    //#####################################################################################

    // this function calculated the Hennesy and johnson solution with 5 eigenvectors
    // it gives exactly the same solutions as selmat for large and small datasets.

    //##################################################################################

    MatrixXd Smat;
    VectorXd vec(3);
    JacobiSVD<MatrixXd> svd(A, ComputeThinU | ComputeThinV);
    // if columns < rows i need to sort u and v matices so Smat is square; cf the matlab function svd(A,0)
    // try|FullPivHouseholderQRPreconditioner if m>n only first n columns of u done and s =n*n
    //m < n — Only the first m columns of V are computed, and S is m-by-m.
    if(A.rows()>A.cols()|A.rows()==A.cols())
    {
        Smat.resize(A.cols(),A.cols());
        Smat =MatrixXd::Zero(A.cols(),A.cols());
        vec.resize(A.cols());
        vec=svd.singularValues();

    }

    if(Acat.rows()<A.cols())
    {
        Smat.resize(Acat.rows(),Acat.rows());
        Smat =MatrixXd::Zero(Acat.rows(),Acat.rows());
        vec.resize(A.rows());
        vec=svd.singularValues();
        // vec.resize(A.rows());
    }
    //#################################################################
    Smat.diagonal()=vec;
    MatrixXd pinv(vec.size(),vec.size());
    pinv = Smat.completeOrthogonalDecomposition().pseudoInverse();


    MatrixXd Ut(svd.matrixU().cols(),svd.matrixU().rows());
    VectorXd Hj5pred(6);

    Ut=svd.matrixU().leftCols(5).transpose();




    Hj5pred=((F*svd.matrixV().leftCols(5)*pinv.topRows(5).leftCols(5))*(Ut))*target;
    emit progress((runs*10)+10);
   // cout<<"hj5"<<Hj5pred<<endl;
    return Hj5pred;
}
/*void Selcon::removeOne(MatrixXd &Amatrix, MatrixXd &Fmatrix, bool &RD, bool& XVal )
{
    XValidation=XVal;
    VectorXd Q(Amatrix.rows());
    MatrixXd Atrunc(Amatrix.rows(), Amatrix.cols()-1);
    MatrixXd Ftrunc(Fmatrix.rows(), Fmatrix.cols()-1);

   // cout<<"Amatrix"<<endl<<Amatrix<<endl;
   // cout<<"Fmatrix"<<endl<<Fmatrix<<endl;
    //remove each spectrum and use as target
    // send target and truncated matrices to selcons and retreicve sel3 solution
  //08/04/2020###############################################################################
    //check both martices have same number of rows
    if(Amatrix.cols()!= Fmatrix.cols())
    {
        QMessageBox msgBox;
        msgBox.setText("The CD data matrix and structure matrix are not compatible.\nPlease check via edit function");

        msgBox.exec();
        return;
    }
    //#####################################################################################
  QProgressDialog *progress= new QProgressDialog("Cross Validating...", "Abort", 0, 100);
 // connect(progress, SIGNAL(canceled()), this, SLOT(Cancel()));
  progress->setWindowModality(Qt::WindowModal);
  progress->setModal(true);
  progress->setValue(1);
  progress->setWindowFlags(Qt::WindowStaysOnTopHint);
  progress->show();

  for(int i =1; i<Amatrix.cols()+1; ++i)
   //    for(int i =1; i<4; ++i)
  {
      int prog = (i*100)/Amatrix.cols();

      if (progress->wasCanceled())
      {

          progress->show();

          break;

      }
      progress->setValue(prog);

      qApp->processEvents();

      // emit Xvalprogress(prog);
      //######################################
      //Atrunc=Amatrix.block(0,i,Amatrix.rows(),Amatrix.cols()-1);
      Atrunc.rightCols(Amatrix.cols()-i)=Amatrix.rightCols(Amatrix.cols()-i);
      Atrunc.leftCols(i-1)=Amatrix.leftCols(i-1);
      Ftrunc.rightCols(Fmatrix.cols()-i)=Fmatrix.rightCols(Fmatrix.cols()-i);
      Ftrunc.leftCols(i-1)=Fmatrix.leftCols(i-1);
      Q =Amatrix.col(i-1);
      qDebug()<<"protcount"<<i;
    //  cout<<"atrunc"<<endl<<Atrunc<<endl;
      //cout<<"Q"<<endl<<Q<<endl;
      getMatrix(Atrunc, Ftrunc, Q);

      if (progress->wasCanceled())
          break;
  }


}*/
void Selcon::Cancel()
{
   // QString lbl =" Wait until reactivation";

    QMessageBox msg;
    msg.setText("wait for program to reactivate. May take a few seconds");
    msg.exec();
}

